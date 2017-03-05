//new add stw
pte_t* get_page_point(struct mm_struct* mm, unsigned long virt)//4级页表，virt为虚拟地址的偏移量
{
	pgd_t* pgd = pgd_offset(mm, virt);
	if (pgd_none(*pgd) || pgd_bad(*pgd))
		return NULL;
	pud_t* pud = pud_offset(pgd, virt);
	if (pud_none(*pud) || pud_bad(*pud))
		return NULL;
	pmd_t* pmd = pmd_offset(pud, virt);
	if (pmd_none(*pmd) || pmd_bad(*pmd))
		return NULL;
	pte_t* pte = pte_offset_map(pmd, virt);
	if (!pte) return NULL;
	return pte;
}


//new add stw
asmlinkage int sys_mysyscall(void)
{	
	struct task_struct *p;
	for (p = &init_task; next_task(p) != &init_task; p = next_task(p))
	{
		if (p->mm && p->mm->mmap)//mm_struct的首地址与虚存区链表首地址
		{
			struct mm_struct *mm = p->mm;
			struct vm_area_struct *vma;
			unsigned long vpage;
			int dirty_page = 0;
			for (vma = mm->mmap; vma; vma = vma->vm_next)//遍历这个进程的所有虚存区
				for (vpage = vma->vm_start; vpage < vma->vm_end; vpage += PAGE_SIZE)//遍历这个虚存区的每一页
				{
					pte_t* pte = get_page_point(mm, vpage);
					if (pte)
					{
						if (pte_dirty(*pte)) dirty_page++;
					}
				}
			printk("$Pid:%d   $Dirty page(1):%d   $Dirty page(2):%d   $Page faults:%lu\n",p->pid, dirty_page, p->nr_dirtied, p->pf);
		}
		else 
		printk("$Pid:%d   $Dirty page(1):0   $Dirty page(2):%d   $Page faults:%lu\n",p->pid, p->nr_dirtied, p->pf);
	}
		
	printk("Total page faults: %lu\n", pfcount);	
	printk("Current process pid: %d\n", current->pid);
	printk("Page faults in current process: %lu\n", current->pf);
	return 0;	
}
