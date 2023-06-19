#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Mikey
typedef vector < vector < int > > MatrixInt;

struct PLAYER_NAME : public Player {

  /**
   * Factory: returns a new instance of this class.
   * Do not modify this function.
   */
  static Player* factory () {
    return new PLAYER_NAME;
  }


  /**
   * Types and attributes for your player can be defined here.
   */

  vector < Dir > directions = {Right, Left, Down, Up};

  pair <Dir,Dir> desfer_cami(Pos ini, Pos fin, map < Pos ,pair <Pos, Dir> >& Prev){
    pair <Dir,Dir> dir_ficha;
    while (ini != fin){
      dir_ficha.second = dir_ficha.first;
      dir_ficha.first = Prev[ini].second;
      ini = Prev[ini].first;
    }
    return dir_ficha;
  }

  bool pos_area(Pos act, int area, Pos nova){
    return (nova.i <= act.i+area and nova.i >= act.i-area and nova.j >= act.j-area and nova.j <= act.j+area);
  }

  void bfs_analitzar(int myid){

    Pos act = unit(myid).pos;
    queue < Pos > Q;
    map < Pos ,pair <Pos, Dir> > Prev;
    MatrixInt distance(board_rows(), vector <int> (board_cols(),  -1));

    Q.push(act);
    distance[act.i][act.j] = 0;
    Pos p;

    while (not Q.empty()){

        p = Q.front();
        Q.pop();
        int cellid = cell(p).id;

        if (distance[p.i][p.j] == 1 and cellid != -1 and unit(cellid).type == Dead) return;

        if (cell(p).food) {
          pair <Dir,Dir> dir_ficha = desfer_cami(p, act, Prev);
          move(myid, dir_ficha.first);
          return;
        }

        if (pos_area(act, 7, p) and cellid != -1 and unit(cellid).type == Zombie){
          pair <Dir,Dir> dir_ficha = desfer_cami(p, act, Prev);
          estrategia_zombie(myid, p, distance[p.i][p.j], dir_ficha);
          return;
        }

        if (pos_area(act, 7, p) and cellid != -1 and unit(cellid).player != me() and unit(cellid).type == Alive){
          pair <Dir,Dir> dir_ficha = desfer_cami(p, act, Prev);
          estrategia_player(myid, p, distance[p.i][p.j], dir_ficha);
          return;
        }

        for (int i = 0; i < 4; ++i){
          Pos new_pos = p+directions[i];
          if (pos_ok(new_pos) and cell(new_pos).type != Waste and distance[new_pos.i][new_pos.j] == -1){
              Q.push(new_pos);
              Prev[new_pos] = make_pair(p, directions[i]);
              distance[new_pos.i][new_pos.j] = distance[p.i][p.j] + 1;
          }
        }
    }
    return;
  }

  bool pos_movible(Pos p){
    return (pos_ok(p) and cell(p).type != Waste and (cell(p).id == -1 or unit(cell(p).id).type != Dead));
  }

  void allunyar(int id, pair<Dir, Dir> direccio){
    Pos act = unit(id).pos;

    if (direccio.first == Right){
      if (pos_movible(act+Left)) move(id, Left);
      else if (direccio.second == Down and pos_movible(act+Up)) move(id, Up);
      else if (direccio.second == Up and pos_movible(act+Down)) move(id, Down);
    }
    else if (direccio.first == Left){
      if (pos_movible(act+Right)) move(id, Right);
      else if (direccio.second == Down and pos_movible(act+Up)) move(id, Up);
      else if (direccio.second == Up and pos_movible(act+Down)) move(id, Down);
    }
    else if (direccio.first == Up){
      if (pos_movible(act+Down)) move(id, Down);
      else if (direccio.second == Left and pos_movible(act+Right)) move(id, Right);
      else if (direccio.second == Right and pos_movible(act+Left)) move(id, Left);
    }
    else if (direccio.first == Down){
      if (pos_movible(act+Up)) move(id, Up);
      else if (direccio.second == Left and pos_movible(act+Right)) move(id, Right);
      else if (direccio.second == Right and pos_movible(act+Left)) move(id, Left);
    }
  }

  void estrategia_zombie(int id, Pos perill, int dist, pair<Dir, Dir> direccio){
    Pos act = unit(id).pos;
    int resi = perill.i-act.i;
    int resj = perill.j-act.j;

    if (dist == 2){
      if (abs(resi) == abs(resj)) allunyar(id, direccio);
    }

    else if (dist == 3){
      if (abs(resi) == 0 or abs(resj) == 0) move(id, direccio.first);
    }

    else move(id, direccio.first);
  }

  void estrategia_player(int id, Pos perill, int dist, pair<Dir, Dir> direccio){
    if (dist > 2) move(id, direccio.first);
    else {
      int equip_cont = cell(perill).owner;
      double forca_total = strength(me())+strength(equip_cont);
      double prob_meva = 0.3+0.7*(strength(me())/forca_total);
      if (prob_meva >= 0.5) move(id, direccio.first);
      else allunyar(id, direccio);
    }
  }

  /**
   * Play method, invoked once per each round.
   */
  virtual void play () {
    double st = status(me());                               // out of time don't do nothing
    if (st >= 0.9) return;
    vector<int> alive = alive_units(me());                  // vector of alive units

    for (int id : alive) {
      bfs_analitzar(id);
    }

  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
