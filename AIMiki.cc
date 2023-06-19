#include "Player.hh"


/**
 * Write the name of your player and save this file
 * with the same name and .cc extension.
 */
#define PLAYER_NAME Miki
typedef vector < vector < bool > > MatrixBool;
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


  bool desfer_cami(Pos ini, Pos fin, pair <Dir, Dir>& dir_ficha, map < Pos ,pair <Pos, Dir> >& Prev, bool perill){
    while (ini != fin){
      dir_ficha.second = dir_ficha.first;
      dir_ficha.first = Prev[ini].second;
      ini = Prev[ini].first;
    }
    return perill;
  }

  bool bfs_analitzar(Pos act, int& dist, pair <Dir, Dir>& dir_ficha, Pos& trobat){

    queue < Pos > Q;
    MatrixBool visited(board_rows (),  vector <bool> (board_cols(),  false));
    map < Pos ,pair <Pos, Dir> > Prev;
    vector < Dir > directions = {Right, Left, Down, Up};
    MatrixInt distance(board_rows(), vector <int> (board_cols(),  -1));

    Q.push(act);
    distance[act.i][act.j] = 0;
    visited[act.i][act.j] = true;
    Pos p;

    while (not Q.empty()){

        p = Q.front();
        Q.pop();
        int cellid = cell(p).id;

        if (cell(p).food) {
          dist = distance[p.i][p.j];
          trobat = p;
          return desfer_cami(p, act, dir_ficha, Prev, false);
        }

        else if (cellid != -1 and unit(cellid).player != me() and (unit(cellid).type == Zombie or unit(cellid).type == Alive)){
          dist = distance[p.i][p.j];
          trobat = p;
          return desfer_cami(p, act, dir_ficha, Prev, true);
        }

        for (int i = 0; i < 4; ++i){
          Pos new_pos = p+directions[i];
          if (pos_ok(new_pos) and cell(new_pos).type != Waste and not visited[new_pos.i][new_pos.j]){
              Q.push(new_pos);
              Prev[new_pos] = make_pair(p, directions[i]);
              distance[new_pos.i][new_pos.j] = distance[p.i][p.j] + 1;
              visited[new_pos.i][new_pos.j] = true;
          }
        }
    }
    return false;
  }

  bool pos_movible_escapar(Pos p){
    return (pos_ok(p) and cell(p).type != Waste and cell(p).id == -1);
  }

  void allunyar_zombie(int id, pair<Dir, Dir> direccio){
    Pos act = unit(id).pos;

    if (direccio.first == Right){
      if (pos_movible_escapar(act+Left)) move(id, Left);
      else if (direccio.second == Down and pos_movible_escapar(act+Up)) move(id, Up);
      else if (direccio.second == Up and pos_movible_escapar(act+Down)) move(id, Down);
    }
    else if (direccio.first == Left){
      if (pos_movible_escapar(act+Right)) move(id, Right);
      else if (direccio.second == Down and pos_movible_escapar(act+Up)) move(id, Up);
      else if (direccio.second == Up and pos_movible_escapar(act+Down)) move(id, Down);
    }
    else if (direccio.first == Up){
      if (pos_movible_escapar(act+Down)) move(id, Down);
      else if (direccio.second == Left and pos_movible_escapar(act+Right)) move(id, Right);
      else if (direccio.second == Right and pos_movible_escapar(act+Left)) move(id, Left);
    }
    else if (direccio.first == Down){
      if (pos_movible_escapar(act+Up)) move(id, Up);
      else if (direccio.second == Left and pos_movible_escapar(act+Right)) move(id, Right);
      else if (direccio.second == Right and pos_movible_escapar(act+Left)) move(id, Left);
    }
  }

  void estrategia_zombie(int id, Pos perill, int dist, pair<Dir, Dir> direccio){
    Pos act = unit(id).pos;
    int resi = perill.i-act.i;
    int resj = perill.j-act.j;

    if (dist == 2){
      //cerr << "dist2";
      if (abs(resi) == abs(resj)) /*cerr << "allunyar" << endl, */allunyar_zombie(id, direccio);
      //else cerr << "quiet" << endl;
    }

    else if (dist == 3){
      //cerr << "dist3";
      if (abs(resi) == 0 or abs(resj) == 0) /*cerr << "apropar" << endl, */move(id, direccio.first);
      //else cerr << "quiet" << endl;
    }

    else /*cerr << "dist 1 o > 3 apropar" << endl, */move(id, direccio.first);
  }

  void estrategia_player(int id, pair<Dir, Dir> direccio, int dist, Pos perill){
    if (dist > 2) move(id, direccio.first);
    else {
      int equip_cont = cell(perill).owner;
      double força_total = strength(me())+strength(equip_cont);
      double prob_meva = 0.3+0.7*(strength(me())/força_total);
      if (prob_meva >= 0.5) move(id, direccio.first);
      else allunyar_zombie(id, direccio);
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
      Pos ficha_trobada;
      int dist = 0;
      pair<Dir, Dir> direccio;

      if(bfs_analitzar(unit(id).pos, dist, direccio, ficha_trobada)){
          int perill_id = cell(ficha_trobada).id;
          if (unit(perill_id).type == Zombie){
             estrategia_zombie(id, ficha_trobada, dist, direccio);
          }
          else estrategia_player(id, direccio, dist, ficha_trobada);
      }
      else move(id,  direccio.first);
    }

  }

};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);
