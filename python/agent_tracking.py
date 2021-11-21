import os
import json
from cellworld_py import *

class New_episode_parameters(Json_object):
    def __init__(self, subject="", experiment="", episode=0, occlusions="", destination_folder=""):
        check_type(subject, str, "wrong type for subject")
        check_type(experiment, str, "wrong type for experiment")
        check_type(episode, int, "wrong type for episode")
        check_type(occlusions, str, "wrong type for occlusions")
        check_type(destination_folder, str, "wrong type for destination_folder")
        self.subject = str(subject)
        self.experiment = str(experiment)
        self.episode = int(episode)
        self.occlusions = str(occlusions)
        self.destination_folder = str(destination_folder)


class Agent_tracking:
    @staticmethod
    def port():
        default_port = 4000
        if os.environ.get("CELLWORLD_AGENT_TRACKING_PORT"):
            try:
                return int(os.environ.get("CELLWORLD_AGENT_TRACKING_PORT"))
            finally:
                pass
        return default_port

    def __init__(self, ip="127.0.0.1", port=None):
        if not port:
            port = Agent_tracking.port()
        self.ip = ip
        self.port = port
        self.registered = False
        self.call_back = None
        self.client = None
        self.episode_in_progress = True

    def __new_step__(self, message):
        try:
            step = message.get_body(Step)
            if self.call_back:
                self.call_back(step)
        finally:
            pass

    def __run__(self, cmd, parameters="", new_connection=True, leave_open=False):
        try:
            if new_connection:
                self.client = Message_client(self.ip, self.port)
                self.client.start()
            self.client.connection.send(Message(cmd, parameters))
            t = Time_out(5)
            while not self.client.pending_messages.contains(cmd + "_result") and t:
                pass
            if not leave_open:
                self.client.stop()
                self.client.connection.close()
                self.client = None
            if self.client.pending_messages.get_message(cmd + "_result").body == "ok":
                return True
            else:
                return False
        except:
            return False

    def register_consumer(self, call_back):
        check_types(call_back, [types.FunctionType, types.MethodType], "incorrect type for call_back")
        self.call_back = call_back
        self.__run__("register_consumer", leave_open=True)
        self.registered = True

    def unregister_consumer(self):
        self.__run__("unregister_consumer", new_connection=False)
        self.registered = False

    def new_episode(self, subject, experiment, episode, occlusions, destination_folder, register=True, call_back=None):
        if register:
            self.register_consumer(call_back)
        if self.registered:
            body = New_episode_parameters(subject, experiment, episode, occlusions, destination_folder)
            self.client.router.add_route("step$", self.__new_step__, Step)
            if self.__run__("new_episode", body, new_connection=False, leave_open=True):
                self.episode_in_progress = True
        else:
            return False
        return True

    def end_episode(self, unregister=True):
        if self.episode_in_progress:
            if self.__run__("end_episode", new_connection=False, leave_open=True):
                self.episode_in_progress = False
            else:
                return False
        else:
            return False
        if unregister:
            return self.unregister_consumer()
        else:
            return True


    def reset_cameras(self):
        return self.__run__("reset_cameras")

    def new_experiment(self, name):
        return self.__run__("new_experiment", name)

    def update_backgrounds(self):
        return self.__run__("update_background")

    def show_occlusions(self, occlusions_configuration):
        return self.__run__("show_occlusions", occlusions_configuration)

    def hide_occlusions(self):
        return self.__run__("hide_occlusions")

