import { Injectable, Logger } from '@nestjs/common';
import { Socket } from 'socket.io';

@Injectable()
export class SocketService {
  public readonly connectedClients: Map<string, Socket> = new Map();

  private logger: Logger = new Logger('SocketService');

  handleConnection(socket: Socket): void {
    const clientId = socket.handshake.query.id ? socket.handshake.query.id.toString() : socket.id;
    this.connectedClients.set(clientId, socket);
    Logger.log("client " + clientId);

    socket.on('disconnect', () => {
      this.connectedClients.delete(clientId);
      this.logger.log(`Client Disconnected: ${clientId}`);
    });

    // Handle other events and messages from the client
  }



  // Add more methods for handling events, messages, etc.
}