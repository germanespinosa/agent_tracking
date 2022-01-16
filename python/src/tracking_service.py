import os

from tcp_messages import MessageServer, Message
from cellworld import *


class TrackingService (MessageServer):
    def __init__(self):
        MessageServer.__init__(self)
        self.world_info = World_info()
        self.registered_consumers = []
        #consumer
        self.router.add_route("^register_consumer$", self.register_consumer)
        self.router.add_route("^unregister_consumer$", self.unregister_consumer)
        #world
        self.router.add_route("^set_world$", self.set_world, World_info)
        self.router.add_route("^get_world$", self.get_world)

    # routes
    # consumer
    def register_consumer(self, message: Message) -> bool:
        print("register_consumer")
        self.registered_consumers.append(message._source)
        return True

    def unregister_consumer(self, message: Message) -> bool:
        print("unregister_consumer")
        self.registered_consumers.remove(message._source)
        return True

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
        self.send_update(Message(step.agent_name + "_step", step))

    def send_update(self, message: Message):
        to_remove = []
        for consumer in self.registered_consumers:
            try:
                consumer.send(message)
            except:
                to_remove.append(consumer)
        for consumer in to_remove:
            self.registered_consumers.remove(consumer)
