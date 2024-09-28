import { Injectable } from '@nestjs/common';
import { Vector } from 'vector-math';


@Injectable()
export class LocalizeService {
    public car: Vector = new Vector(0, 0, 0);
    public block: Vector = new Vector(0, 0, 0);
    public person: Vector = new Vector(0, 0, 0);

    public markers: Vector[] = [
        new Vector(0, 0, 0),
        new Vector(0, 0, 0),
        new Vector(0, 0, 0),
        new Vector(0, 0, 0)
    ]
}
