#include <kbd.h>
#include <i386.h>
#include <xeroslib.h>
#include <xeroskernel.h>
unsigned int kbtoa(unsigned char code);
int echoq;
int tempb[4];
// Opens the keyboard depending on the type
unsigned int kbopen(pcb* p, int device_no, int major) {
	if (major != ECHO && major != NONECHO) {
		return -1;
	}
	if (major == ECHO) {
		echoq = ECHO;
	}
	if (major == NONECHO) {
		echoq = NONECHO;
	}
	enable_irq(1, 0);
	p->files[major].status = DEVICE_OPEN;
	return major;
}
unsigned int kbclose(pcb* p, int major) {
	enable_irq(1, 1);
	p->files[major].status = DEVICE_CLOSED;
	return 0;
}

// Cannot write
unsigned int kbwrite(pcb* p) {
	return -1;
}
// Reads input. The issue is likely to be here
unsigned int kbread(pcb* p, int fdes, unsigned char *buf, int size) {
	// check the port's low order bit
	if (inb(CNTRPORT) & 0x01) {
		int i = 0;
		while (i < 3) {
			char letter = kbtoa(inb(READPORT)); // reading from another port
			if (letter == ENTER_KEY) { // if "ENTER" add to buffer and stop
				memset(buf, letter, size);
				return 0;
			}
			if (letter == EOF || letter == EOT) { // Control D or end of file 
				return 0;
			}
			memset(buf, letter, size);
			if (echoq == ECHO) { // if ECHO print characters
				kprintf(letter);
			}
			i++;
		}
		return 0;

	}
}

// Not tested. reading using interrupt
unsigned int kbreadint(void) {
	// check the port's low order bit
	if (inb(CNTRPORT) & 0x01) {
		int i = 0;
		while (i < 4) {
			char letter = kbtoa(inb(READPORT)); // reading from another port
			if (letter == ENTER_KEY) { // if "ENTER" add to buffer and stop
				tempb[i] = letter;
				kprintf(letter);
			}
			if (letter == EOF) { // Control D or end of file
				tempb[i] = letter;
				kprintf(letter);
			}
			if (letter == EOT) {
				break;
			}

			if (echoq == ECHO) { // if ECHO print characters
				tempb[i] = letter;
				kprintf(letter);
			}

			i++;
		}
		
		return 0;
	}
}

// switches modes echo/nonecho, redefines end of file
unsigned int kbioctl(unsigned long command) {
	switch (command) {
		case 53:
			#undef EOF
			#define EOF command
			return 0;
		case 55:
			echoq = NONECHO;
			return 0;
		case 56:
			echoq = ECHO;
			return 0;
		default:
			return -1;
	}
		
}

static int extchar(unsigned char code){
	return state &= ~EXTENDED;
}

unsigned int kbtoa(unsigned char code){
	unsigned int    ch;

	if (state & EXTENDED)
		return extchar(code);
	if (code & KEY_UP) {
		switch (code & 0x7f) {
		case LSHIFT:
		case RSHIFT:
			state &= ~INSHIFT;
			break;
		case CAPSL:
			kprintf("Capslock off detected\n");
			state &= ~CAPSLOCK;
			break;
		case LCTL:
			state &= ~INCTL;
			break;
		case LMETA:
			state &= ~INMETA;
			break;
		}

		return NOCHAR;
	}


	/* check for special keys */
	switch (code) {
	case LSHIFT:
	case RSHIFT:
		state |= INSHIFT;
		kprintf("shift detected!\n");
		return NOCHAR;
	case CAPSL:
		state |= CAPSLOCK;
		kprintf("Capslock ON detected!\n");
		return NOCHAR;
	case LCTL:
		state |= INCTL;
		return NOCHAR;
	case LMETA:
		state |= INMETA;
		return NOCHAR;
	case EXTESC:
		state |= EXTENDED;
		return NOCHAR;
	}

	ch = NOCHAR;

	if (code < sizeof(kbcode)) {
		if (state & CAPSLOCK) {
			ch = kbshift[code];
		}	
		else {
			ch = kbcode[code];
		}
			
	}
	if (state & INSHIFT) {
		if (code >= sizeof(kbshift)) {
			return NOCHAR;
		}
			
		if (state & CAPSLOCK) {
			ch = kbcode[code];
		}
		else {
			ch = kbshift[code];
		}
			
	}
	if (state & INCTL) {
		if (code >= sizeof(kbctl)) { 
			return NOCHAR; 
		}
		ch = kbctl[code];
	}
	if (state & INMETA) {
		ch += 0x80;
	}
		
	return ch;
}

