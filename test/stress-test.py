import os
import subprocess
import argparse
import random
import sys

#============================== Helper Functions ==============================# 

def match(a, b, msg, args):
    if (a == b):                            # Print that they match
        if(args.verbose):
            print(msg, " Match!")
    else:                                   # Print that they don't match
        if(args.verbose):
            print(msg, " Failure")
        exit()

def run_trial(test_paths, mount_paths, args):
    for mount_path, test_path in zip(mount_paths, test_paths):
        test_is_dir = os.path.isdir(test_path)
        test_is_file = os.path.isfile(test_path) 

        mount_is_dir = os.path.isdir(mount_path)
        mount_is_file = os.path.isfile(mount_path) 

        mount_name = mount_path.split('/')[-1]
        test_name  = test_path.split('/')[-1]

        if(args.verbose):
            print(test_path, " vs ", mount_path)
            print(test_name, " vs ", mount_name)

        both_dir = test_is_dir & mount_is_dir
        both_file = test_is_file & mount_is_file

        match(both_dir | both_file, True, "Type", args)

        if (both_dir):
            test_ls = os.listdir(test_path)
            mount_ls = os.listdir(mount_path)
            match(test_ls, mount_ls, "Directory Content", args)
            
        if (both_file):                        
            # Compare file contents
            try:
                if (args.content) :
                    test_content = open(test_path, 'r').read()
                    mount_content= open(mount_path, 'r').read()
                # compare mount stat and mount length
                mount_stat = os.stat(mount_path)
                test_stat  = os.stat(test_path) 
                match(mount_stat.st_size, test_stat.st_size, "File Stat", args)
                # match(test_content, mount_content, "File Content", args)
            except UnicodeDecodeError: 
                print("Could not read")
    
        if(args.verbose):
            print('-'*8)

def run_sorted_trial(test_paths, mount_paths, args):
    test_paths.sort()
    mount_paths.sort()
    run_trial(test_paths, mount_paths, args)

def run_random_trial(test_paths, mount_paths, args):
    combined = list(zip(test_paths, mount_paths))
    random.shuffle(combined)
    test_paths[:], mount_paths[:] = zip(*combined)
    run_trial(test_paths, mount_paths, args)

#==================================== Main ====================================#

def main():
    parser = argparse.ArgumentParser(description='Mutation test the WOFS against a correct filesystem')
    parser.add_argument('-t','--trials', type=int, help='Number of trials')
    parser.add_argument('-v','--verbose', action='store_true', help='show each test')
    parser.add_argument('-c','--content', action='store_true', help='compare file content')
    parser.add_argument('-r','--randomize', action='store_true', help='shuffle access to each file')
    parser.set_defaults(trials=1)

    args = parser.parse_args()

    mount_point = "../src/plz/tensorflow"
    true_path = "./tensorflow"

    find_command = 'find '
    test_out = os.popen(find_command + true_path).read().split('\n')[0:-1]
    mount_out = os.popen(find_command + mount_point).read().split('\n')[0:-1]

    match(len(test_out), len(mount_out), "# items", args)
    for n in range(args.trials):
        if(args.verbose):
            print("Trial: ", n)

        if (args.randomize):
            run_random_trial(test_out, mount_out, args)
        else:
            run_sorted_trial(test_out, mount_out, args)

    print("Stress Test Successful!")

if __name__ == '__main__':
    main()