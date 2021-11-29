
from agent_tracking_py import Agent_tracking

from cellworld_py import Timer


def show_step(message):
    print(message)


tracker = Agent_tracking()
tracker.register_consumer(show_step)
t = Timer(3)
while t:
    pass
tracker.unregister_consumer()






# from agent_tracking import Agent_tracking, New_episode_message
#
# from time import sleep
# import os
# from cellworld_py import *
#
# a = Agent_tracking()
#
# sleep(3)
# a.show_occlusions("10_05")
# sleep(3)
# a.hide_occlusions()
# sleep(3)
# a.new_experiment("test_experiment")
# sleep(3)
# a.new_episode("MMM4", "TEST", 0, "10_05", os.getcwd())
# sleep(10)
# a.end_episode()
# sleep(3)
# a.update_backgrounds()
# sleep(3)
# a.reset_cameras()

