import os

from tcp_messages import MessageServer, Message
from cellworld import *


class TrackingService (MessageServer):
    def __init__(self):
        MessageServer.__init__(self)
        self.world_info = World_info()
        #consumer
        self.allow_subscription = True
        self.router.add_route("send_step", self.send_step, Step)

    @staticmethod
    def port() -> int:
        default_port = 4510
        if os.environ.get("CELLWORLD_AGENT_TRACKING_PORT"):
            try:
                return int(os.environ.get("CELLWORLD_AGENT_TRACKING_PORT"))
            finally:
                pass
        return default_port

    def send_step(self, step: Step):
        self.broadcast_subscribed(Message(step.agent_name + "_step", step))
