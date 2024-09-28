import { log } from 'console';
import { WebSocketGateway, OnGatewayConnection, WebSocketServer, OnGatewayDisconnect, SubscribeMessage } from '@nestjs/websockets';
import { Socket } from 'socket.io';
import { SocketService } from './socket.service';
import { Logger } from '@nestjs/common';
import { LocalizeService } from 'src/localize/localize/localize.service';
import { Vector } from 'vector-math';

class Path{
	id: number;
	points: [{x: number, y: number, r: number}]
}

@WebSocketGateway()
export class SocketGateway implements OnGatewayConnection {
	@WebSocketServer()
	private server: Socket;
	private logger: Logger = new Logger('SocketGateway');

	public lastPath: Path;

	constructor(
		private readonly socketService: SocketService,
		private readonly localizeService: LocalizeService
	) { }

	handleConnection(socket: Socket): void {
		this.socketService.handleConnection(socket);
		this.logger.log(socket.handshake.query.id);
	}

	id(client: Socket): string {
		return client.handshake.query.id.toString()
	}

	@SubscribeMessage('model')
	handleDirection(client: Socket, payload: { path: Path }) {
		if(payload.path.id != this.lastPath.id)
			this.lastPath = payload.path;
	}

	// { type: string, id?: number, x: number, y: number, r: number }

	@SubscribeMessage('vision')
	handlePosition(client: Socket, payload: { id: number, x: number, y: number, r: number }) {
		const clientId: string = this.id(client);

		if (payload.id == 0) {
			this.localizeService.car = new Vector(payload.x, payload.y, payload.r)
			this.logger.log(this.localizeService.car.i);
		}
		else if(payload.id == 1) {
			this.localizeService.car = new Vector(payload.x, payload.y, payload.r)
		}
		else if(payload.id == 2) {
			this.localizeService.car = new Vector(payload.x, payload.y, payload.r)
		}
		else if(this.localizeService.markers[payload.id]){
			this.localizeService.markers[payload.id] = new Vector(payload.x, payload.y, payload.r)
		}
	}
}