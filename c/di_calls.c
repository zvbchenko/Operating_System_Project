// DEVICE calls
// INITIALIZATION


#include <xeroskernel.h>
#include <xeroslib.h>


fdt_entry DEVICE_TABLE[2];

// Initializing both echoing and non-echoing keyboards
extern void di_init(void) {
	// ECHO-friendly keyboard:
	fdt_entry* kbde = &DEVICE_TABLE[ECHO];
	kbde->dvnum = ECHO;
	kbde->dvname = "ECHO-KEYBRD";
	kbde->dvopen = (funcptr)kbopen;
	kbde->dvwrite = (funcptr)kbwrite;
	kbde->dvread = (funcptr)kbread;
	kbde->dvclose = (funcptr)kbclose;

	// not Echo-friendly keyboard:
	fdt_entry* kbd = &DEVICE_TABLE[NONECHO];
	kbd->dvnum = NONECHO;
	kbd->dvname = "ECHO-KEYBRD";
	kbd->dvopen = (funcptr)kbopen;
	kbd->dvwrite = (funcptr)kbwrite;
	kbd->dvread = (funcptr)kbread;
	kbd->dvclose = (funcptr)kbclose;

}
  /* di_open allocates a FDT entry	for device*/
extern int di_open(pcb* p, int device_no) {
		
	if (device_no >= 2 || device_no < 0) {
		return -1;
	}
	
	fdt_entry* device = &DEVICE_TABLE[device_no];
	int major = p->files[device_no].major;
	return device->dvopen(p, device_no, major);
}

/* di_close closes the device	*/
extern int di_close(pcb* p, int fdes) {
	if (!(fdes < 4 && fdes > -1)) {
		return -1;
	}
	file_descriptor * fd = &p->files[fdes];
	int major = fd->major;
	fdt_entry* device = &DEVICE_TABLE[major];
	return device->dvclose(p,  major);
}
/* writes by sending task to device*/
extern int di_write(pcb* p, int fdes, unsigned char *buf, int size) {
	if (!(fdes < 4 && fdes > -1)) {
		return -1;
	}
	file_descriptor * fd = &p->files[fdes];
	int major = fd->major;
	fdt_entry* device = &DEVICE_TABLE[major];
	return device->dvwrite(p);
}
/* reads by sending task to device*/
extern int di_read(pcb* p, int fdes, unsigned char *buf, int size) {
	if (!(fdes < 4 && fdes > -1)) {
		return -1;
	}
	file_descriptor * fd = &p->files[fdes];
	int major = fd->major;
	fdt_entry* device = &DEVICE_TABLE[major];
	return device->dvread(p, device, major, buf, size);
}
/* controls input/output using device */
extern int di_iotcl(pcb* p, int fdes, unsigned long command, ...) {
	if (!(fdes < 4 && fdes > -1)) {
		return -1;
	}
	file_descriptor * fd = &p->files[fdes];
	int major = fd->major;
	fdt_entry* device = &DEVICE_TABLE[major];
	return device->dvcntl(command);
}
