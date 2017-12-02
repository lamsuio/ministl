#!groovy

podTemplate(label: 'ministl', containers: [
    containerTemplate(name: 'clang', image: 'goby/clang')
  ]) {

    node('ministl') {
        stage('build and test') {
            //git url: 'https://github.com/goby/first-blood.git'
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
