plugins {
    kotlin("multiplatform") version "1.9.21" apply false
    id("org.jetbrains.compose") version "1.5.11" apply false
    id("com.android.application") version "8.1.4" apply false
}

group = "com.example.calendarexample"
version = "1.0-SNAPSHOT"

allprojects {
    repositories {
        google()
        mavenCentral()
        maven("https://maven.pkg.jetbrains.space/public/p/compose/dev")
    }
}
