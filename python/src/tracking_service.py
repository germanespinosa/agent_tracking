import os

from tcp_messages import MessageServer, Message
from cellworld import *


class TrackingService (MessageServer):
    def __init__(self):
        MessageServer.__init__(self)
        self.world_info = World_info()
        #consumer
        self.allow_subscription = True
        self.router.add_route("^register_consumer$", self.__subscribe_connection__)
        self.router.add_route("^unregister_consumer$", self.__unsubscribe_connection__)
        #world
        self.router.add_route("^set_world$", self.set_world, World_info)
        self.router.add_route("^get_world$", self.get_world)

    #world
    def get_world(self) -> World_info:
        print("get_world")
        return self.world_info

    def set_world(self, world_info: World_info):
        print("set_world")
        self.world_info = world_info
        self.send_update(Message("world_update", self.world_info))

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
