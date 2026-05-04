
#include "../../includes/container.hpp"

void Server::sendresponse(Client *cl)
{
  if (cl->getState() == PROCESSING)
  {
    DEBUG_INFO("Response");
    Response res(cl->getReq());
    if (_status_error)
    {
      cl->setWriteBuffer(res.build(_status_error));
      //std::cout << cl->getWriteBuffer() << std::endl;
    }
    else
      cl->setWriteBuffer(res.build());
    cl->setState(SENDING);
    /*EPOLLOUT fires on client_fd:
    TODO: PROCESSING:
    build response headers
    DELETE → execute delete, transition to RESPONDING
    GET    → find file, transition to RESPONDING
    POST   → move tmp_file to final location, transition to RESPONDING
    */
  }
  else if (cl->getState() == SENDING)
  {
    /*
    Sending:
      DELETE/POST → response is small, write once → transition to DONE
      GET →
        read next chunk from file
        write chunk to socket
        if more chunks → stay in RESPONDING
        if file done   → transition to DONE

    DONE:
       disconnect()
    TODO: read from file and send to socket
      - build response headers and send to socket
      - check if isResponseLarge, if it read from fd chunk by chunk and send to socket
    */
    
    // send data to client and close connection after done

    send(cl->getFd(), cl->getWriteBuffer().c_str(), cl->getWriteBuffer().length(), 0);
    cl->setState(DONE); 
  }
}

void Server::sendFromFile(Client *cl, int file_fd)
{
    if (cl->getWriteBuffer().empty())
    {
        char buf[CHUNK_SIZE];
        ssize_t bytes = read(file_fd, buf, CHUNK_SIZE);
        if (bytes < 0)
        {
            DEBUG_WARN("sendFromFile: error with read");
            return;
        }
        if (bytes == 0)
        {
            cl->setState(DONE);
            return;
        }
        ssize_t bytes_sended = send(cl->getFd(), buf, bytes, 0);
        if (bytes_sended <= 0)
        {
            DEBUG_WARN("sendFromFile: error/disconnect on send");
            return;
        }
        // NOTE:
        // writeBuf holds only the unsent remainder from the last read.
        // writeOffset tracks position within that remainder on subsequent partial sends.
        cl->setWriteBuffer(std::string(buf + bytes_sended, bytes - bytes_sended));
        return;
    }
    ssize_t bytes_sended = send(cl->getFd(),
        cl->getWriteBuffer().c_str() + cl->getWriteOffset(),
        cl->getWriteBuffer().size() - cl->getWriteOffset(), 0);
    if (bytes_sended <= 0)
    {
        DEBUG_WARN("sendFromFile: error/disconnect on send");
        return;
    }
    cl->advanceWriteOffset(bytes_sended);
    if (cl->getWriteOffset() == cl->getWriteBuffer().size())
        cl->clearWriteBuffer();// set offset to 0
}
