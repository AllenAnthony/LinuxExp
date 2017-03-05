ssize_t new_sync_write_crypt(struct file *filp, const char __user *buf, size_t len, loff_t *ppos)
{
		char* mybuf = buf;
        for(int i=0;i<len;i++)//在此处添加对长度为len的buf数据进行加密（简单移位密码，将每个字符值+25）
		{
			mybuf[i]+=25;
		}
        printk("haha encrypt %ld\n", len);
		return new_sync_write(filp, mybuf, len, ppos);//调用默认的写函数，把加密数据写入
}

ssize_t new_sync_read_crypt(struct file *filp, char __user *buf, size_t len, loff_t *ppos)
{
        int i;
		//先调用默认的读函数读取文件数据
        ssize_t ret = new_sync_read(filp, buf, len, ppos);
		//此处添加对文件的解密（简单移位解密，将每个字符值-25）
		for(i=0;i<len;i++)
		{
			buf[i]-=25;
		}
        printk("haha encrypt %ld\n", len);
        return ret;
}

static ssize_t new_sync_write(struct file *filp, const char __user *buf, size_t len, loff_t *ppos)
{
	struct iovec iov = { .iov_base = (void __user *)buf, .iov_len = len };
	struct kiocb kiocb;
	struct iov_iter iter;
	ssize_t ret;

	init_sync_kiocb(&kiocb, filp);
	kiocb.ki_pos = *ppos;
	iov_iter_init(&iter, WRITE, &iov, 1, len);

	ret = filp->f_op->write_iter(&kiocb, &iter);
	BUG_ON(ret == -EIOCBQUEUED);
	if (ret > 0)
		*ppos = kiocb.ki_pos;
	return ret;
}

static ssize_t new_sync_read(struct file *filp, char __user *buf, size_t len, loff_t *ppos)
{
	struct iovec iov = { .iov_base = buf, .iov_len = len };
	struct kiocb kiocb;
	struct iov_iter iter;
	ssize_t ret;

	init_sync_kiocb(&kiocb, filp);
	kiocb.ki_pos = *ppos;
	iov_iter_init(&iter, READ, &iov, 1, len);

	ret = filp->f_op->read_iter(&kiocb, &iter);
	BUG_ON(ret == -EIOCBQUEUED);
	*ppos = kiocb.ki_pos;
	return ret;
}