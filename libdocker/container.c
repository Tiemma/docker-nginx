#include "docker.h"

int main() {

  fprintf(stderr, "curl_version: %s\n", curl_version());

  DOCKER *docker = docker_init("v1.40");

  if (docker) {
    CURLcode response = docker_get(
        docker,
        "http://v1.40/containers/json?filters={\"network\":[\"nginx\"]}");

    if (response == CURLE_OK) {
      fprintf(stderr, "%s\n", docker_buffer(docker));
    }

    docker_destroy(docker);
  } else {
    fprintf(stderr, "ERROR: Failed to get get a docker client!\n");
  }

  return 0;
}
