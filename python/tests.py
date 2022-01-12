from cellworld_tracking import TrackingService, TrackingClient
from cellworld import Timer, Step, Coordinates, Location

service = TrackingService()

service.start(TrackingService.port())

client = TrackingClient()

client.connect("127.0.0.1")


def print_step(step):
    print(step)


client.register_consumer(print_step)

while len(service.registered_consumers) == 0:
    pass

print("consumer registered")

time_stamp = Timer()
for i in range(100):
    print(i, end=" ")
    service.send_step(Step(time_stamp.to_seconds(), "predator", i, Coordinates(i, -i), Location(float(i)/10, float(-i)/10), float(i)/30, "data:1"))
    t = Timer(.015)
    while t:
        pass
print("")
client.unregister_consumer()

while len(service.registered_consumers) == 1:
    pass

print("consumer unregistered")

client.disconnect()
service.stop()

