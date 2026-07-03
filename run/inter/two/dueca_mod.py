## -*-python-*-
## dueca_mod.py: created with DUECA version 4.2.2
## Created on: 26-Jun-2025

## this is an example dueca_mod.py file, for you to start out with and adapt
## according to your needs. Note that you need a dueca_mod.py file only for the
## node with number 0

## in general, it is a good idea to clearly document your set up
## this is an excellent place.

## [nodes] node set-up
ecs_node = 0    # experiment control station, node 0 with gui

## priority set-up
# normal nodes: 0 administration
#               1 logging
#               2 simulation, unpackers
#               3 communication
#               4 ticker

# administration priority. Run the interface and logging here
admin_priority = dueca.PrioritySpec(0, 0)

# logging prio. Keep this free from time-critical other processes
log_priority = dueca.PrioritySpec(1, 0)

# priority of simulation, just above log
sim_priority = dueca.PrioritySpec(2, 0)

## [priority] nodes with a different priority scheme

# control loading node has 0, 1, 2 and 3 as above and furthermore
#               4 stick priority
#               5 ticker priority
# priority of the stick. Higher than prio of communication
# stick_priority = dueca.PrioritySpec(4, 0)

## timing set-up

# simulation process, this is normally 100, giving 100 Hz timing
sim_timing = dueca.TimeSpec(0, 100)

## for now, display on 50 Hz
display_timing = dueca.TimeSpec(0, 200)

## log a bit more economical, 25 Hz
log_timing = dueca.TimeSpec(0, 400)

## [timing] nodes with different timing set-up

# timing of the stick calculations. Assuming 100 usec ticks, this gives 2500 Hz
# stick_timing = dueca.TimeSpec(0, 4)

## the name for the main entity to create. Note that you can create as
## many entities as you want, usually one is enough. Adjust as appropriate
entity_name = "two"

## ---------------------------------------------------------------------
### the modules needed for dueca itself
if this_node_id == ecs_node:

    # create a list of modules:
    DUECA_mods = []
    DUECA_mods.append(dueca.Module("dusime", "", admin_priority))
    DUECA_mods.append(dueca.Module("dueca-view", "", admin_priority).param(
        position_size=(0,360),
        glade_file="dusime.ui"
    ))
    DUECA_mods.append(dueca.Module("activity-view", "", admin_priority))
    DUECA_mods.append(dueca.Module("timing-view", "", admin_priority))
    DUECA_mods.append(dueca.Module("log-view", "", admin_priority))
    DUECA_mods.append(dueca.Module("channel-view", "", admin_priority))
    # uncomment for web-based graph, see DUECA documentation
    # DUECA_mods.append(dueca.Module("config-storage", "", admin_priority))

    if no_of_nodes > 1 and not classic_ip:
        DUECA_mods.append(dueca.Module("net-view", "", admin_priority))

    # remove the quotes to enable DUSIME initial condition recording and
    # setting, and simulation recording and replay
    for e in (entity_name,):
        DUECA_mods.append(
            dueca.Module("initials-inventory", e, admin_priority).param(
                # reference_file=f"initials-{e}.toml",
                store_file=f"initials-{e}-%Y%m%d_%H%M.toml"))
        DUECA_mods.append(
            dueca.Module("replay-master", e, admin_priority).param(
                # reference_files=f"recordings-{e}.ddff",
                store_files=f"recordings-{e}-%Y%m%d_%H%M%S.ddff"))

    # create the DUECA entity with that list
    DUECA_entity = dueca.Entity("dueca", DUECA_mods)

## ---------------------------------------------------------------------
# modules for your project
mymods = []

## [modules] modules to be created in node 0 ("ecs")
if this_node_id == ecs_node:

    # placeholder when no modules are defined, remove if desired
    # create a writer in the world channel
    mymods.append(dueca.Module(
        'write-unified', '', sim_priority).param(
            set_timing=sim_timing,
            add_blip=entity_name,
            place_blip=[0, 0, 0.1, 0.1]
        )
    )

    mymods.append(dueca.Module(
        'read-unified', '', sim_priority).param(
            set_timing=sim_timing)
    )

    # placeholder when no modules are defined, remove if desired
    mymods.append(dueca.Module(
        'channel-replicator-peer', "", comm_prio).param(
            master_information_channel="ReplicatorPeerAcknowledge://central",
            config_url="ws://localhost:8765/config",
            #timing_gain=0.002
            )
        )
    filer = dueca.ReplayFiler(entity_name)

# then combine in an entity
if mymods:
    myentity = dueca.Entity(entity_name, mymods)
