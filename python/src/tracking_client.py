import types
from .tracking_service import TrackingService
from tcp_messages import MessageClient, Message
from cellworld import *


class TrackingClient (MessageClient):
    def __init__(self):
        MessageClient.__init__(self)
        self.registered_consumer = False
        self.on_step = None
        self.current_states = dict()
        self.router.add_route("_step$", self.__process_step__, Step)

    def __process_step__(self, step: Step):
        try:
            self.current_states[step.agent_name] = step.copy()
            if self.on_step:
                self.on_step(step)
        finally:
            pass

    def connect(self, ip: str = "127.0.0.1") -> bool:
        return MessageClient.connect(self, ip, TrackingService.port())

    def register_consumer(self, call_back=None):
        if call_back:
            check_types(call_back, [types.FunctionType, types.MethodType], "incorrect type for call_back")
        self.on_step = call_back
        response = self.send_request(Message("register_consumer"), 0)
        self.registered_consumer = (response.body == "success")
        return self.registered_consumer

    def unregister_consumer(self):
        response = self.send_request(Message("unregister_consumer"), 0)
        if response.body == "success":
            self.registered = False
            return True
        else:
            return False

    def get_current_state(self, agent_name: str) -> Step:
        return self.current_states[agent_name]

    def contains_agent_state(self, agent_name: str) -> bool:
        return agent_name in self.current_states

    def set_throughput(self, throughput) -> bool:
        return self.send_request(Message("set_throughput", throughput)).get_body(bool)
