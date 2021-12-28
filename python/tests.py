from src import Agent_tracking
from cellworld import Timer


def show_step(message):
    print(message)


tracker = Agent_tracking()
tracker.register_consumer(show_step)
t = Timer(3)
while t:
    pass
tracker.unregister_consumer()

