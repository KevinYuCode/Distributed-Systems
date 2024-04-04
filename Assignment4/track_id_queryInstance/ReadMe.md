# Simple Cyclone DDS system
This system is an update to the psr esamples to show  the multiple queues used with keyed topics.
The command lien argument is the queue depth, and it shows that each id as its own queue depth.

## Restrictions

subscriber uses a simple polling loop, with 2 seconds between each poss. Does not use waitsets.
