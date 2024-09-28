import { Module } from '@nestjs/common';
import { AppController } from './app.controller';
import { AppService } from './app.service';
import { SocketModule } from './socket/socket/socket.module';
import { LocalizeService } from './localize/localize/localize.service';

@Module({
  imports: [SocketModule],
  controllers: [AppController],
  providers: [AppService, LocalizeService],
})
export class AppModule {}
