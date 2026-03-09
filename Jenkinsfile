pipeline {
    agent { label 'hil' }

    stages {

        stage('Checkout') {
            steps {
                checkout scm
            }
        }

        stage('Install PlatformIO') {
            steps {
                bat 'pip install -U platformio'
            }
        }

        stage('Build HIL Firmware') {
            steps {
                bat 'pio run -e hil_test'
            }
        }

        stage('Upload Firmware') {
            steps {
                bat 'pio run -e hil_test -t upload'
                sleep 3
            }
        }

        stage('Run HIL Tests') {
            steps {
                bat 'pytest --junitxml=pytest-report.xml'
            }
        }
    }

    post {
        always {
            junit 'pytest-report.xml'
        }
    }
}
