plugins {
    id("buildlogic.java-conventions")
}

var junitVersion = "5.10.2"
var junitLauncherVersion = "1.10.2"

dependencies {
    testImplementation(project(":xpc"))
    testImplementation("org.junit.jupiter:junit-jupiter:$junitVersion")
    testRuntimeOnly("org.junit.platform:junit-platform-launcher:$junitLauncherVersion")
}

tasks.named<Test>("test") {
    useJUnitPlatform()

    maxHeapSize = "1G"

    testLogging {
        events("passed")
    }
}
