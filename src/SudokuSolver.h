#ifndef SUDOKUSOLVER_H
#define SUDOKUSOLVER_H

#include <iostream>
#include <string>
#include <typeinfo>
#include <vector>
#include <algorithm>

using namespace std;

class Posibles {
   vector<bool> _b;
public:
   Posibles() : _b(9, true) {}
   bool   activo(int i)  const { return _b[i-1]; }
   int    num_activos()  const { return count(_b.begin(), _b.end(), true); }
   void   elimina(int i)       { _b[i-1] = false; }
   int    val()          const {
      vector<bool>::const_iterator it = find(_b.begin(), _b.end(), true);
      return (it != _b.end() ? 1 + (it - _b.begin()) : -1);
   }

   string str(int ancho) const {
       string s(ancho, ' ');
       int k = 0;
       for (int i = 1; i <= 9; i++) {
          if (activo(i)) s[k++] = '0' + i;
       }
       return s;
    }
};


class SudokuSolver {
        vector<Posibles> _celdas;
        static vector< vector<int> > _grupo, _vecinos, _grupos_de;

        bool     elimina(int k, int val);
    public:
        SudokuSolver(string s);
        SudokuSolver(vector<int> s);
        static void inicializa();
        static SudokuSolver* soluciona(SudokuSolver *S);

        Posibles posibles(int k) const { return _celdas[k]; }
        bool resuelto() const;
        bool asigna(int k, int val);
        int  menos_posibilidades() const;
        void escribe(ostream& o) const;
};
#endif // SUDOKUSOLVER_H
