# Example wrapper for my network sir code github.com/pholme/sir
# it illustrates how to properly seed the sir code with 64-bit random numbers


from sys import argv
from subprocess import check_output
from random import getrandbits

#  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #

if __name__ == "__main__":

	if len(argv) != 2:
		print('usage: python3 run_sir.py [nwk file]')
		exit()

	for i in range(-3,4):
		out = check_output(['./sir',argv[1],str(2**i),str(getrandbits(64))])
		a = out.decode().split('\n')
		b = a[0].split()
		c = a[1].split()
		print(str(2**i),b[-2],b[-1].strip('()'),c[-2],c[-1].strip('()'))

#  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #  #