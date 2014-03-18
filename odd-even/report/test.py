
import os

for j in [256000,128000,64000,32000,16000,8000,4000,2000,1000]:
	for i in range(1,11):
		cmd = "bsub -a intelmpi -q short -o output.%%J -e error.%%J -n %d  mpirun.lsf ../serial_version/run/odd-even-sort %d" %(i*12, j *10)
		print cmd
		os.system(cmd)
