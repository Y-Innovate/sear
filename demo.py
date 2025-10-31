import curses
import json
from sear import sear

def searInAndOut(stdscr, req):
    pad = curses.newpad(5000, 100)

    stdscr.refresh()

    jsonstrin = json.dumps(req, indent=4)

    resp = sear(req)

    jsonstrout = json.dumps(resp.result, indent=4)

    pad.addstr("In:\n\n")

    pad.addstr(jsonstrin)
    pad.addstr("\n\n")

    pad.addstr("Out:\n\n")

    pad.addstr(jsonstrout)
    pad.addstr("\n\n")

    maxy, maxx = stdscr.getmaxyx()
    xleft = 0
    ytop = 0

    choice = ""

    while choice != 120 and choice != 88 and choice != 113 and choice != 81:
        pad.refresh(ytop, xleft, 2, 2, maxy-4, maxx-4)

        choice = stdscr.getch()

        if choice == curses.KEY_DOWN:
            ytop += 1
        elif choice == curses.KEY_UP:
            ytop -= 1
        elif choice == curses.KEY_LEFT:
            xleft -= 1
        elif choice == curses.KEY_RIGHT:
            xleft += 1

        if xleft < 0:
            yleft = 0
        if ytop < 0:
            ytop = 0

def displayUser(stdscr):
    global glb

    stdscr.addstr(f"====== Display {glb["user"]["userid"]} ======\n\n")

    req = {}
    req["operation"] = "extract"
    req["admin_type"] = "user"
    req["userid"] = glb["user"]["userid"]

    searInAndOut(stdscr, req)

def createUser(stdscr):
    global glb

    stdscr.addstr(f"====== Create {glb["user"]["userid"]} ======\n\n")

    req = {}
    req["operation"] = "add"
    req["admin_type"] = "user"
    req["userid"] = glb["user"]["userid"]
    req["traits"] = {}
    req["traits"]["base:name"] = glb["user"]["name"]

    searInAndOut(stdscr, req)

def changePW(stdscr):
    global glb

    stdscr.addstr(f"====== Change {glb["user"]["userid"]} password ======\n\n")

    req = {}
    req["operation"] = "alter"
    req["admin_type"] = "user"
    req["userid"] = glb["user"]["userid"]
    req["traits"] = {}
    req["traits"]["base:password"] = glb["user"]["password"]

    searInAndOut(stdscr, req)

def deleteUser(stdscr):
    global glb

    stdscr.addstr(f"====== Delete {glb["user"]["userid"]} ======\n\n")

    req = {}
    req["operation"] = "delete"
    req["admin_type"] = "user"
    req["userid"] = glb["user"]["userid"]

    searInAndOut(stdscr, req)

def searchUsers(stdscr):
    global glb

    stdscr.addstr(f"====== Search users ======\n\n")

    req = {}
    req["operation"] = "search"
    req["admin_type"] = "user"

    searInAndOut(stdscr, req)

def displayGroup(stdscr):
    global glb

    stdscr.addstr(f"====== Display {glb["group"]["group"]} ======\n\n")

    req = {}
    req["operation"] = "extract"
    req["admin_type"] = "group"
    req["group"] = glb["group"]["group"]

    searInAndOut(stdscr, req)

def createGroup(stdscr):
    global glb

    stdscr.addstr(f"====== Create {glb["group"]["group"]} ======\n\n")

    req = {}
    req["operation"] = "add"
    req["admin_type"] = "group"
    req["group"] = glb["group"]["group"]

    searInAndOut(stdscr, req)

def connectUser(stdscr):
    global glb

    stdscr.addstr(f"====== Connect {glb["user"]["userid"]} to {glb["group"]["group"]} ======\n\n")

    req = {}
    req["operation"] = "alter"
    req["admin_type"] = "group-connection"
    req["userid"] = glb["user"]["userid"]
    req["group"] = glb["group"]["group"]

    searInAndOut(stdscr, req)

def removeUser(stdscr):
    global glb

    stdscr.addstr(f"====== Remove {glb["user"]["userid"]} from {glb["group"]["group"]} ======\n\n")

    req = {}
    req["operation"] = "delete"
    req["admin_type"] = "group-connection"
    req["userid"] = glb["user"]["userid"]
    req["group"] = glb["group"]["group"]

    searInAndOut(stdscr, req)

def deleteGroup(stdscr):
    global glb

    stdscr.addstr(f"====== Delete {glb["group"]["group"]} ======\n\n")

    req = {}
    req["operation"] = "delete"
    req["admin_type"] = "group"
    req["group"] = glb["group"]["group"]

    searInAndOut(stdscr, req)

def searchGroups(stdscr):
    global glb

    stdscr.addstr(f"====== Search groups ======\n\n")

    req = {}
    req["operation"] = "search"
    req["admin_type"] = "group"

    searInAndOut(stdscr, req)

def displayResource(stdscr):
    global glb

    stdscr.addstr(f"====== Display {glb["resource"]["resource"]} ======\n\n")

    req = {}
    req["operation"] = "extract"
    req["admin_type"] = "resource"
    req["resource"] = glb["resource"]["resource"]
    req["class"] = glb["resource"]["class"]

    searInAndOut(stdscr, req)

def createResource(stdscr):
    global glb

    stdscr.addstr(f"====== Create {glb["resource"]["resource"]} ======\n\n")

    req = {}
    req["operation"] = "add"
    req["admin_type"] = "resource"
    req["resource"] = glb["resource"]["resource"]
    req["class"] = glb["resource"]["class"]

    searInAndOut(stdscr, req)

def permitResource(stdscr):
    global glb

    stdscr.addstr(f"====== Permit {glb["group"]["group"]} access to {glb["resource"]["resource"]} ======\n\n")

    req = {}
    req["operation"] = "alter"
    req["admin_type"] = "permission"
    req["resource"] = glb["resource"]["resource"]
    req["class"] = glb["resource"]["class"]
    req["group"] = glb["group"]["group"]
    req["traits"] = {}
    req["traits"]["base:access"] = "READ"

    searInAndOut(stdscr, req)

def deleteResource(stdscr):
    global glb

    stdscr.addstr(f"====== Delete {glb["resource"]["resource"]} ======\n\n")

    req = {}
    req["operation"] = "delete"
    req["admin_type"] = "resource"
    req["resource"] = glb["resource"]["resource"]
    req["class"] = glb["resource"]["class"]

    searInAndOut(stdscr, req)

def searchResources(stdscr):
    global glb

    stdscr.addstr(f"====== Search resources in {glb["resource"]["class"]} ======\n\n")

    req = {}
    req["operation"] = "search"
    req["admin_type"] = "resource"
    req["class"] = glb["resource"]["class"]

    searInAndOut(stdscr, req)

def userSwitchBoard(stdscr):
    choice = ""

    while choice.lower() != "x" and choice.lower() != "q":
        stdscr.clear()

        stdscr.addstr("====== User switch board ======\n\n")
        stdscr.addstr("  1. display user\n")
        stdscr.addstr("  2. create user\n")
        stdscr.addstr("  3. change pw\n")
        stdscr.addstr("  4. delete user\n")
        stdscr.addstr("  5. search users\n")
        stdscr.addstr("  x. exit\n")

        stdscr.refresh()

        choice = stdscr.getkey()

        stdscr.clear()

        if choice == "1":
           displayUser(stdscr)
        elif choice == "2":
           createUser(stdscr)
        elif choice == "3":
           changePW(stdscr)
        elif choice == "4":
           deleteUser(stdscr)
        elif choice == "5":
           searchUsers(stdscr)

def groupSwitchBoard(stdscr):
    choice = ""

    while choice.lower() != "x" and choice.lower() != "q":
        stdscr.clear()

        stdscr.addstr("====== Group switch board ======\n\n")
        stdscr.addstr("  1. display group\n")
        stdscr.addstr("  2. create group\n")
        stdscr.addstr("  3. add user to group\n")
        stdscr.addstr("  4. remove user from group\n")
        stdscr.addstr("  5. delete group\n")
        stdscr.addstr("  6. search groups\n")
        stdscr.addstr("  x. exit\n")

        stdscr.refresh()

        choice = stdscr.getkey()

        stdscr.clear()

        if choice == "1":
           displayGroup(stdscr)
        elif choice == "2":
           createGroup(stdscr)
        elif choice == "3":
           connectUser(stdscr)
        elif choice == "4":
           removeUser(stdscr)
        elif choice == "5":
           deleteGroup(stdscr)
        elif choice == "6":
           searchGroups(stdscr)

def resourceSwitchBoard(stdscr):
    choice = ""

    while choice.lower() != "x" and choice.lower() != "q":
        stdscr.clear()

        stdscr.addstr("====== Resource switch board ======\n\n")
        stdscr.addstr("  1. display resource\n")
        stdscr.addstr("  2. create resource\n")
        stdscr.addstr("  3. permit group\n")
        stdscr.addstr("  4. delete resource\n")
        stdscr.addstr("  5. search resources\n")
        stdscr.addstr("  x. exit\n")

        stdscr.refresh()

        choice = stdscr.getkey()

        stdscr.clear()

        if choice == "1":
           displayResource(stdscr)
        elif choice == "2":
           createResource(stdscr)
        elif choice == "3":
           permitResource(stdscr)
        elif choice == "4":
           deleteResource(stdscr)
        elif choice == "5":
           searchResources(stdscr)

def mainSwitchBoard(stdscr):
    stdscr.scrollok(True)

    choice = ""

    while choice.lower() != "x" and choice.lower() != "q":
        stdscr.clear()

        stdscr.addstr("====== Main switch board ======\n\n")

        stdscr.addstr("Make a choice:\n")
        stdscr.addstr("  1. user\n")
        stdscr.addstr("  2. group\n")
        stdscr.addstr("  3. resource\n")
        stdscr.addstr("  x. exit\n")

        stdscr.refresh()

        choice = stdscr.getkey()

        stdscr.clear()

        if choice == "1":
           userSwitchBoard(stdscr)
        elif choice == "2":
           groupSwitchBoard(stdscr)
        elif choice == "3":
           resourceSwitchBoard(stdscr)

glb = {}

glb["user"] = {}
glb["user"]["userid"]   = "GSUK01"
glb["user"]["name"]     = "GSUK user 01"
glb["user"]["password"] = "GSUK1234"

glb["group"] = {}
glb["group"]["group"] = "GSUKGRP"

glb["resource"] = {}
glb["resource"]["resource"] = "GSUK.RESOURCE"
glb["resource"]["class"] = "FACILITY"

curses.wrapper(mainSwitchBoard)
