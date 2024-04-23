plugins {
    id("buildlogic.java-conventions")
    application
}

group = "gov.nasa.xpc.examples"

description = "XPC Discovery Example"

dependencies {
    implementation(project(":xpc"))
}
