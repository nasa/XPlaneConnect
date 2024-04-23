plugins {
    id("buildlogic.java-conventions")
    id("org.openjfx.javafxplugin") version "0.1.0"
    id("org.javamodularity.moduleplugin") version "1.8.12"
    application
}

group = "gov.nasa.xpc.examples"

description = "XPC Example Playback"


dependencies {
    implementation(project(":xpc"))
}

java {
    modularity.inferModulePath.set(false)
}

javafx {
    version = "11"
    modules("javafx.graphics")
    configuration = "implementation"
}
