"""
This file demonstrates how to receive step messages from the tracking svc
"""
from cellworld import Timer, Step
from cellworld_tracking import TrackingClient


class AgentData:
    """
    Stores steps (info from tracker) for each agent
    """
    def __init__(self, agent_name: str):
        self.is_valid = None
        self.step = Step()                      # step object - step is a class in experiment.py
        self.step.agent_name = agent_name


def on_step(step):
    """
    Updates steps and predator behavior
    """
    if step.agent_name == "predator":
        predator.is_valid = Timer(time_out)
        predator.step = step


predator = AgentData("predator")
time_out = 1.0


tracker = TrackingClient()
if tracker.connect("127.0.0.1"):
    print("connected to tracker")
else:
    print("failed to connect to tracker")
tracker.set_request_time_out(5000)
tracker.subscribe()                  # subscribe to recieve messages from tracker
tracker.set_throughput(5)            # specify number of messages recieved per second
tracker.on_step = on_step


while True:
    if predator.is_valid:
        print(predator.step)
    else:
        print("no step")


tracker.unsubscribe()