#!/usr/bin/env groovy

podTemplate(label: 'ministl', containers: [
    containerTemplate(name: 'clang', image: 'goby/clang', ttyEnabled: true)
  ]) {

    node('ministl') {
        stage('build and test') {
            git url: 'https://github.com/goby/ministl.git'
            container('clang') {
                stage('Build') {
                    sh """
                    mkdir -p build
                    cd build && cmake .. && make && cd ..
                    """
                }
                stage('Test') {
                    sh """
                    valgrind ./build/main_test
                    """
                }
            }
        }
    }
}
