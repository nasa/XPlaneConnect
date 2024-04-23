plugins {
    `java-library`
    `maven-publish`
}

repositories {
    mavenLocal()
    maven {
        url = uri("https://oss.sonatype.org/content/repositories/snapshots")
    }

    maven {
        url = uri("https://repo.maven.apache.org/maven2/")
    }
}

group = "gov.nasa.xpc"
version = "1.4.0-SNAPSHOT"
java.sourceCompatibility = JavaVersion.VERSION_1_8

publishing {
    repositories {
        maven {
            name = "GitHubPackages"
            url = uri("https://maven.pkg.github.com/nasa/XPlaneConnect")
            credentials {
                username = project.findProperty("gpr.user") as String? ?:
                    System.getenv("GITHUB_ACTOR")
                password = project.findProperty("gpr.key") as String? ?:
                    System.getenv("GITHUB_TOKEN")
            }
        }
    }

    publications {
        register<MavenPublication>("gpr") {
            from(components["java"])
        }
    }

//    publications.create<MavenPublication>("maven") {
//        from(components["java"])
//    }
}

tasks.withType<JavaCompile>() {
    options.encoding = "UTF-8"
}

tasks.withType<Javadoc>() {
    options.encoding = "UTF-8"
}
