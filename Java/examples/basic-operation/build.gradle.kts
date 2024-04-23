plugins {
    id("buildlogic.java-conventions")
    application
}

group = "gov.nasa.xpc.examples"

description = "XPC Example Basic Operation"

dependencies {
    implementation(project(":xpc"))
}
