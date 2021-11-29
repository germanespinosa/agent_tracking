import httpimport
with httpimport.remote_repo(["cellworld_py_setup"], "https://raw.githubusercontent.com/germanespinosa/cellworld_py/master/"):
    import cellworld_py_setup
cellworld_py_setup.install(version="1.3")

import zipfile
import requests
import os

def download(url, path=None):
    print (url)
    web_get = requests.get(url)
    if not path:
        path = url.split("/")[-1]
    with open(path, "wb") as f:
        f.write(web_get.content)


def extract(file_name):
    zip_ref = zipfile.ZipFile(file_name)  # create zipfile object
    zip_ref.extractall(".")  # extract file to dir
    zip_ref.close()  # close file


def install(version="", force=False):
    is_installed = False
    try:
        import cellworld_py
    except:
        pass
    try:
        if cellworld_py.version() == version:
            is_installed = True
    except:
        pass

    if is_installed and not force:
        return

    version_v = ""

    if version:
        version_v = "_" + version

    try:
        download("https://raw.githubusercontent.com/germanespinosa/agent_tracking/master/python/build/agent_tracking_py" + version_v + ".zip", "agent_tracking_py.zip")
    except:
        raise ValueError('agent_tracking_py version ' + version + 'installation failed')

    extract("agent_tracking_py.zip")
    os.remove("agent_tracking_py.zip")
    print('agent_tracking_py version ' + version + ' installed')
