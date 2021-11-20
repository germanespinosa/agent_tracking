import httpimport
with httpimport.remote_repo(["cellworld_py_setup"], "https://raw.githubusercontent.com/germanespinosa/cellworld_py/master/"):
    import cellworld_py_setup
cellworld_py_setup.install(version="1.1", force=True)
from agent_tracking import Agent_tracking, New_episode_message
from time import sleep
import os
from cellworld_py import Coordinates

a = Agent_tracking()

sleep(3)
a.show_occlusions("10_05")
sleep(3)
a.hide_occlusions()
sleep(3)
a.new_experiment("test_experiment")
sleep(3)
a.new_episode("MMM4", "TEST", 0, "10_05", os.getcwd())
sleep(10)
a.end_episode()
sleep(3)
a.update_backgrounds()
sleep(3)
a.reset_cameras()


