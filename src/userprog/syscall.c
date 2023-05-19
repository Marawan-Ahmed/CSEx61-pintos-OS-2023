#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "list.h"
#include "process.h"

static void syscall_handler (struct intr_frame *);

struct proc_file {
	struct file* ptr;
	int fd;
	struct list_elem elem;
};

int exec_proc(char *file_name)
{
	acquire_filesys_lock();
	char * fn_cp = malloc (strlen(file_name)+1);
	  strlcpy(fn_cp, file_name, strlen(file_name)+1);
	  
	  char * save_ptr;
	  fn_cp = strtok_r(fn_cp," ",&save_ptr);

	 struct file* f = filesys_open (fn_cp);

	  if(f==NULL)
	  {
	  	release_filesys_lock();
	  	return -1;
	  }
	  else
	  {
	  	file_close(f);
	  	release_filesys_lock();
	  	return process_execute(file_name);
	  }
}
void exit_proc(int status)
{
	//printf("Exit : %s %d %d\n",thread_current()->name, thread_current()->tid, status);
	struct list_elem *e;

      for (e = list_begin (&thread_current()->parent->child_proc); e != list_end (&thread_current()->parent->child_proc);
           e = list_next (e))
        {
          struct child *f = list_entry (e, struct child, elem);
          if(f->tid == thread_current()->tid)
          {
          	f->used = true;
          	f->exit_error = status;
          }
        }


	thread_current()->exit_error = status;

	if(thread_current()->parent->waitingon == thread_current()->tid)
		sema_up(&thread_current()->parent->child_lock);

	thread_exit();
}

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  int * p = f->esp;

  int system_call = * p;

	switch (system_call)
	{
		case SYS_HALT:
	  	shutdown_power_off();
		break;

		case SYS_EXIT:
  		exit_proc(*(p+1));
		break;

		case SYS_EXEC:
		  f->eax = exec_proc(*(p+1));
		break;

		case SYS_WAIT:
  		f->eax = process_wait(*(p+1));
		break;

		case SYS_CREATE:
      acquire_filesys_lock();
      f->eax = filesys_create(*(p+4),*(p+5));
      release_filesys_lock();
		break;

		case SYS_REMOVE:
      acquire_filesys_lock();
      if(filesys_remove(*(p+1))==NULL)
        f->eax = false;
      else
        f->eax = true;
      release_filesys_lock();
		break;

		case SYS_OPEN:
      // acquire_filesys_lock();
      // struct file* fptr = filesys_open (*(p+1));
      // release_filesys_lock();
      // if(fptr==NULL)
      //   f->eax = -1;
      // else
      // {
      //   struct proc_file *pfile = malloc(sizeof(*pfile));
      //   pfile->ptr = fptr;
      //   pfile->fd = thread_current()->fd_count;
      //   thread_current()->fd_count++;
      //   list_push_back (&thread_current()->files, &pfile->elem);
      //   f->eax = pfile->fd;

      // }
		break;

		case SYS_FILESIZE:
      // acquire_filesys_lock();
      // f->eax = file_length (list_search(&thread_current()->files, *(p+1))->ptr);
      // release_filesys_lock();
		break;

		case SYS_READ:
      // if(*(p+5)==0)
      // {
      //   int i;
      //   uint8_t* buffer = *(p+6);
      //   for(i=0;i<*(p+7);i++)
      //     buffer[i] = input_getc();
      //   f->eax = *(p+7);
      // }
      // else
      // {
      //   struct proc_file* fptr = list_search(&thread_current()->files, *(p+5));
      //   if(fptr==NULL)
      //     f->eax=-1;
      //   else
      //   {
      //     acquire_filesys_lock();
      //     f->eax = file_read (fptr->ptr, *(p+6), *(p+7));
      //     release_filesys_lock();
      //   }
      // }
		break;

		case SYS_WRITE:
      // if(*(p+5)==1)
      // {
      //   putbuf(*(p+6),*(p+7));
      //   f->eax = *(p+7);
      // }
      // else
      // {
      //   struct proc_file* fptr = list_search(&thread_current()->files, *(p+5));
      //   if(fptr==NULL)
      //     f->eax=-1;
      //   else
      //   {
      //     acquire_filesys_lock();
      //     f->eax = file_write (fptr->ptr, *(p+6), *(p+7));
      //     release_filesys_lock();
      //   }
      // }
		break;

		case SYS_SEEK:
      // acquire_filesys_lock();
      // file_seek(list_search(&thread_current()->files, *(p+4))->ptr,*(p+5));
      // release_filesys_lock();
		break;

		case SYS_TELL:
      // acquire_filesys_lock();
      // f->eax = file_tell(list_search(&thread_current()->files, *(p+1))->ptr);
      // release_filesys_lock();
		break;

		case SYS_CLOSE:
      // acquire_filesys_lock();
      // close_file(&thread_current()->files,*(p+1));
      // release_filesys_lock();
		break;


		default:
		printf("Default %d\n",*p);
  }
  // printf ("system call!\n");
  // thread_exit ();
}
