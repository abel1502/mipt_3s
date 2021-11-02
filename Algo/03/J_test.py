import subprocess
import io
import random
import string


def test(cmds):
    inData = "{cmds}\n".format(n=len(cmds), cmds='\n'.join(cmds))
    result = subprocess.run("J.exe", input=inData.encode(), stdout=subprocess.PIPE)
    result_ref = subprocess.run("J_ref.exe", input=inData.encode(), stdout=subprocess.PIPE)
    return result.returncode == 0 and list(result.stdout.decode().splitlines()) == list(result_ref.stdout.decode().splitlines())


def genTest(minN=1, maxN=10):
    n = random.randrange(minN, maxN)
    return [''.join(random.choices(string.ascii_lowercase, k=n))]


def stress(count):
    for i in range(count):
        cmds = genTest(maxN=10**3)
        
        if not test(cmds):
            print("[Error]", cmds)
            #break
        if i % 10 == 0:
            print(".")

stress(100)