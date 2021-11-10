import subprocess
import io
import random
import string


def test(cmds):
    inData = "{cmds}\n".format(n=len(cmds), cmds='\n'.join(cmds))
    result = subprocess.run("F.exe", input=inData.encode(), stdout=subprocess.PIPE)
    result_ref = subprocess.run("F2.exe", input=inData.encode(), stdout=subprocess.PIPE)
    return result.returncode == 0 and list(result.stdout.decode().splitlines()) == list(result_ref.stdout.decode().splitlines())


def genTest(minN=1, maxN=10):
    return [str(random.randrange(minN, maxN))]


def stress(count):
    for i in range(count):
        cmds = genTest(minN=10**9, maxN=10**11)
        
        if not test(cmds):
            print("[Error]", cmds)
            #break
        if i % 10 == 0:
            print(".")

stress(100)

# [Error] ['7706137792']
# 3034833237
# 354884462