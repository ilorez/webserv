#include "./includes/container.hpp"

int main()
{
  try {
    Server nginx;
    nginx.run();
  } catch (const ServerException &e) {
    std::cerr << ERROR_MSG << "server: " << e.what() << std::endl;
  } catch (const RequestException &e) {
    std::cerr << ERROR_MSG << "request: "<< e.what() << std::endl;
  } catch (const ResponseException &e) {
    std::cerr << ERROR_MSG << "response: "<< e.what() << std::endl;
  } catch (const std::exception &e) {
    std::cerr << ERROR_MSG << "error: "<< e.what() << std::endl;
  }
}
