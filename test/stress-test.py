import os
import subprocess

VERBOSE_FLAG = True
ITERATIONS = 1

def match(a, b, msg):
    if (a == b):                            # Print that they match
        if(VERBOSE_FLAG):
            print(msg, " Match!")
    else:                                   # Print that they don't match
        if(VERBOSE_FLAG):
            print(msg, " Failure")
        exit()

mount_point = "../src/mountPoint/test/tests"
true_path = "./tests"

find_command = 'find '
test_out = os.popen(find_command + true_path).read().split('\n')[0:-1]
mount_out = os.popen(find_command + mount_point).read().split('\n')[0:-1]

test_out.sort()
mount_out.sort()

match(len(test_out), len(mount_out), "# items")
for n in range(ITERATIONS):
    if(VERBOSE_FLAG):
        print("Trial: ", n)

    for i in range(len(test_out)):
        test_path   = test_out[i]
        test_is_dir = os.path.isdir(test_path)
        test_is_file = os.path.isfile(test_path) 

        mount_path = mount_out[i]
        mount_is_dir = os.path.isdir(mount_path)
        mount_is_file = os.path.isfile(mount_path) 

        mount_name = mount_path.split('/')[-1]
        test_name  = test_path.split('/')[-1]

        if(VERBOSE_FLAG):
            print(test_name, " vs ", mount_name)

        both_dir = test_is_dir & mount_is_dir
        both_file = test_is_file & mount_is_file

        match(both_dir | both_file, True, "Type")

        if (both_dir):
            test_ls = os.listdir(test_path)
            mount_ls = os.listdir(mount_path)
            match(test_ls, mount_ls, "Directory Content")
            

        if (both_file):                        
            # Compare file contents
            test_content = open(test_path, 'r').read()
            mount_content= open(mount_path, 'r').read()
            match(test_content, mount_content, "File Content")

            # compare mount stat and mount length
            mount_stat = os.stat(mount_path)
            test_stat  = os.stat(test_path) 
            match(mount_stat.st_size, test_stat.st_size, "File Content")

        if(VERBOSE_FLAG):
            print('-'*8)

print("Stress Test Successful!")