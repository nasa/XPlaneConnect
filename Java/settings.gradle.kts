rootProject.name = "xpc"

include(":xpc")
include(":tests")
include(":examples")
include(":examples:basic-operation")
include(":examples:continuous-operation")
include(":examples:discovery")
include(":examples:playback")

project(":xpc").projectDir = file("xpc")
project(":tests").projectDir = file("$rootDir/../TestScripts/Java Tests")
project(":examples").projectDir = file("examples")
