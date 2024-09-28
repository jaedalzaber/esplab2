import { Module } from '@nestjs/common';
import { SocketGateway } from './socket.gateway';
import { SocketService } from './socket.service';
import { LocalizeService } from 'src/localize/localize/localize.service';

@Module({
  providers: [SocketGateway, SocketService, LocalizeService],
})
export class SocketModule {}