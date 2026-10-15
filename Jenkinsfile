// エージェントには distbuilder, CMake, C++20 対応コンパイラ, Vulkan 1.4 SDK, Doxygen が必要です.
// テスト結果の記録には Jenkins の JUnit Plugin が必要です.
def runCommand(String command) {
    if (isUnix()) {
        sh command
    } else {
        bat command
    }
}

pipeline {
    agent {
        label 'vulkan && cpp'
    }

    options {
        disableConcurrentBuilds()
    }

    stages {
        stage('Dependencies') {
            steps {
                script {
                    runCommand('distbuilder --export-presets .')
                }
            }
        }

        stage('Configure') {
            steps {
                script {
                    // default の依存設定を利用し, 出力先をワークスペース内に統一します.
                    runCommand('cmake --preset default -B build/jenkins -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=install/jenkins -DVKWP_BUILD_TESTS=ON -DVKWP_BUILD_DOCS=ON -DVKWP_INSTALL=ON')
                }
            }
        }

        stage('Build') {
            steps {
                script {
                    runCommand('cmake --build build/jenkins --config Release --parallel')
                    // doxygen は通常のビルドに含まれないため, 明示的に生成します.
                    runCommand('cmake --build build/jenkins --config Release --target doxygen')
                }
            }
        }

        stage('Test') {
            steps {
                dir('build/jenkins') {
                    script {
                        runCommand('ctest -C Release --output-on-failure --no-tests=error --output-junit test-results.xml')
                    }
                }
            }
            post {
                always {
                    junit testResults: 'build/jenkins/test-results.xml'
                }
            }
        }

        stage('Install') {
            steps {
                script {
                    runCommand('cmake --install build/jenkins --config Release')
                }
                archiveArtifacts artifacts: 'install/jenkins/**', fingerprint: true
            }
        }
    }
}
