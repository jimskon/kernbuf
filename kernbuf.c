#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("James Skon");
MODULE_DESCRIPTION("A kernel module memory buff");
MODULE_VERSION("0.01");
#define DEVICE_NAME "kernbuf"

/* Prototypes for device functions */
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static int major_num;
static int device_open_count = 0;

#define MSG_BUFFER_LEN 256
static char msg_buffer[MSG_BUFFER_LEN];
static char *msg_ptr;

#define PROCFS_MAX_SIZE 1024 
#define PROCFS_NAME "kernbuf" 

/* This structure points to all of the device functions */
static struct file_operations file_ops = {
 .read = device_read,
 .write = device_write,
 .open = device_open,
 .release = device_release
};
/* Input/Output queue - enqueue and dequeue data for input and output */
struct node {
  char *buf;         // points to a buffer of recieved data
  size_t len;       // The total length of the buffer
  size_t offset;    // The offset to unretrieved data
  struct node *next;
};
typedef struct node node;

struct queue {
  node *front;
  node *rear;
};

static struct queue q;

static void init_q(struct queue *q) {
  q->front = NULL;
  q->rear = NULL;
}

/* Add to queue buffer */
static int enqueue_q(struct queue *q, char *buf, ssize_t len) {
  node *n_ptr;
  /* Create new node */
  n_ptr = kzalloc(sizeof(node), GFP_KERNEL);
  if (!n_ptr) {
    return -ENOMEM;
  }

  /* set up new node */
  n_ptr->buf = buf;
  n_ptr->len = len;
  n_ptr->offset = 0;
  n_ptr->next = NULL;
  if (q->rear != NULL) {
    q->rear->next = n_ptr;
    q->rear = n_ptr;
  } else {
    q->front = q->rear = n_ptr;
  }
  return 0;
}

/* Get up to the the next len characters from head */
char *dequeue_q(struct queue *q, size_t len, size_t *rlen) {
  node *head;
  size_t avail;
  char *b_ptr;
  node *n_ptr;
  *rlen = 0;

  if (!q->front)
       return NULL;
  /* remove any leading empty entries */
  while (q->front && q->front->len - q->front->offset == 0) {
      n_ptr = q->front;
      b_ptr = n_ptr->buf;
      q->front = q->front->next;
      kfree(b_ptr);
      kfree(n_ptr);
    
      if (!q->front) {
	q->rear = NULL;
	return NULL;
      }
  }

  /* Now let's get some data */
  head = q->front;

  avail = head->len - head->offset;
  b_ptr = head->buf + head->offset;
  
  if (avail <= len) {
    // return everything we have in this node
    *rlen = avail;
    head->offset = head->len;
  } else {
    // return what was asked for, and adjust buffer
    *rlen = len;
    head->offset += len;
  }
  return b_ptr;
}

void destroy_q(struct queue *q) {
  char *b_ptr;
  node *n_ptr;
  
  while (q->front) {
      n_ptr = q->front;
      b_ptr = n_ptr->buf;
      q->front = q->front->next;
      kfree(b_ptr);
      kfree(n_ptr);
	}
}

/* process a read from the kernel module */
static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset) {
  size_t rlen;
  size_t clen;
  size_t tlen = 0;
  char * d_ptr;
  
  d_ptr = dequeue_q(&q,len,&rlen);
  while (len && rlen && d_ptr) {
    clen = copy_to_user(buffer,d_ptr,rlen);
    buffer += rlen;
    len -= rlen;
    tlen += rlen;
    d_ptr = dequeue_q(&q,len,&rlen);
  }

 return tlen;
}

/* Process a write to the kernal module */
static ssize_t device_write(struct file *flip, const char *buffer, size_t len, loff_t *offset) {
    char * ptr;
    size_t ret;
    /* Create space for the buffer */
    ptr = kzalloc(len, GFP_KERNEL);
    if (!ptr) {
       return -ENOMEM;
    }

    ret = copy_from_user(ptr,buffer,len);
    
    enqueue_q(&q,ptr,len);
	return len;
    printk(KERN_INFO "Char: Received %zu characters from the user\n", len);
    return len;
 
    //printk(KERN_ALERT "This operation is not supported.\n");
 return len;
}

/* Process a device open request */
static int device_open(struct inode *inode, struct file *file) {
 /* If device is open, return busy */
 if (device_open_count) {
 return -EBUSY;
 }
 device_open_count++;
 try_module_get(THIS_MODULE);
 return 0;
}

/* Called when a process closes our device */
static int device_release(struct inode *inode, struct file *file) {
  /* Decrement the open counter and usage count. */
 device_open_count--;
 module_put(THIS_MODULE);
 return 0;
}


static int __init kernbuf_init(void) {
  /* Set up the queue */
  init_q(&q);
  
 /* Set the msg_ptr to the buffer */
 msg_ptr = msg_buffer;
 /* Try to register character device */
 major_num = register_chrdev(0, "kernbuf", &file_ops);
 if (major_num < 0) {
 printk(KERN_ALERT "Could not register device: %d\n", major_num);
 return major_num;
 } else {
 printk(KERN_INFO "kernbuf module loaded with device major number %d\n", major_num);
 return 0;
 }
}

static void __exit kernbuf_exit(void) {
  /* Cleanup on unload */
 destroy_q(&q);
 unregister_chrdev(major_num, DEVICE_NAME);
 printk(KERN_INFO "kernbuf unloaded\n");
}
/* Register module functions */
module_init(kernbuf_init);
module_exit(kernbuf_exit);
