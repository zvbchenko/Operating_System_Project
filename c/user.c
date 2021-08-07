/* user.c : User processes
 */

#include <xeroskernel.h>
#include <xeroslib.h>

void     root( void ) {
/****************************/
	int testsyssighand = syssighandler(1, NULL, NULL);
	kprintf("Test for syssighandler returns %d, expecting -3\n", testsyssighand);
	int testkill = syskill(1, -500);
	kprintf("Test for syskill returns %d, expecting -583\n", testkill);
	int testwait = syswait(40);
	kprintf("Test for wait returns %d, expecting -1\n", testwait);
	int testopen = sysopen(444);
	kprintf("Test for open returns %d, expecting -1\n", testopen);
	int testwrite = syswrite(8, 0, 0);
	kprintf("Test for write returns %d, expecting -1\n", testwrite);
	int testctr = sysioctl(3, 60);
	kprintf("Test for iocontrol returns %d, expecting -1\n", testctr);
	int michaelbuffer[3][10];
	int readtest = sysread(4, &michaelbuffer[0], sizeof(michaelbuffer[0]));
	kprintf("Test for readtest returns %d, expecting -1\n", readtest);
	//char *buff[5] = {"c", "s", "4", "1", "5"};
	//char pass[80];


	char input[256];
	char *inputData = &input[0];

	char pass[256];
	char *userPass = &pass[0];

	char user[6] = { 'c', 's', '4', '1', '5', '\0' };

	char password[16] = { 'E', 'v', 'e', 'r', 'y',
						  'o', 'n', 'e',
						  'g', 'e', 't', 's',
						  'a', 'n',
						  'A' };
	Bool auth = TRUE;

	char userCommand[256];

	char ps[2] = { 'p', 's' };
	char ex[2] = { 'e', 'x' };
	char K = 'k';
	char A = 'a';
	char T = 't';
	do {
		do {
			kprintf("\n\nWelcome to Xeros - an experimental OS");

			sysopen(1);

			kprintf("\nUsername: ");

			sysread(1, inputData, sizeof(char[6]));

			sysioctl(1, 55);
			kprintf("\nPassword: ");

			sysread(1, userPass, sizeof(char[16]));

			sysclose(1);

			for (int i = 0; i < 6; i++) {
				if (input[i] != user[i]) {
					auth = FALSE;
					break;
				}
			}

			for (int i = 0; i < 16; i++) {
				if (pass[i] != password[i]) {
					auth = FALSE;
					break;
				}
			}
		} while (!auth);
	} while (TRUE);

}

