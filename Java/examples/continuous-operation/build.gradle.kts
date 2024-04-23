plugins {
    id("buildlogic.java-conventions")
    application
}

group = "gov.nasa.xpc.examples"

description = "XPC Example Continuous Operation"

dependencies {
    implementation(project(":xpc"))
}
