import sys
from zipfile import ZipFile
import glob

version = ""
if len(sys.argv) > 1:
    version = "_" + sys.argv[1]

with open("./agent_tracking_py/version.py", "w") as v:
    v.write("def version():\n\treturn '" + sys.argv[1] + "'\n")

zipObj = ZipFile('build/agent_tracking_py' + version + '.zip', 'w')

g = glob.glob("./agent_tracking_py/*.py")
for f in g:
    zipObj.write(f)

zipObj.close()
