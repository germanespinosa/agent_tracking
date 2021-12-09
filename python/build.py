import subprocess
import sys
from zipfile import ZipFile
import glob

lib_name = "agent_tracking_py"

version = ""
if len(sys.argv) > 1:
    version = "_" + sys.argv[1]

print("creating new version")
with open("./" + lib_name + "/version.py", "w") as v:
    v.write("def version():\n\treturn '" + sys.argv[1] + "'\n")
print("done!")

print("building library")
lib_file = 'build/' + lib_name + version + '.zip'
zipObj = ZipFile(lib_file, 'w')
g = glob.glob("./" + lib_name + "/*.py")
for f in g:
    zipObj.write(f)

zipObj.close()
print("done!")

print("uploading library" )
p = subprocess.Popen(["bash","-c", "git add " + lib_file + "; git commit " + lib_file + " -m \"build version " + version + "\" ;git push; echo FINISHED"],
                     stdout=subprocess.PIPE,
                     stderr=subprocess.PIPE)
o, e = p.communicate()

while b"FINISHED" not in o:
    pass
print("done!")
