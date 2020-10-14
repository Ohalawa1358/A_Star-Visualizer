#include <iostream>
#include "olcConsoleGameEngine.h"
#include <vector>
#include <cmath>
#include <limits>

class PathFinding : public olcConsoleGameEngine {
public:

    PathFinding() {
        m_sAppName = L"Path Finding";
    }
private:
    struct Node {
        bool bObstical;
        bool bVisited;
        float localDistance = std::numeric_limits<float>::infinity();
        float globalDistance = std::numeric_limits<float>::infinity();
        int x, y;
        Node* parent;
        std::vector<Node*> neighbors;
    };

    Node* nodes = nullptr;
    int nMapHeight = 16;
    int nMapWidth = 16;

    Node* nodeStart;
    Node* nodeEnd;

    float CalcHueristic(Node* p1, Node* p2) {
        return std::abs(p2->x - p1->x) + abs(p2->y - p1->y);
    }

    void A_Star() {

        for (int x = 0; x < nMapWidth; x++) {
            for (int y = 0; y < nMapHeight; y++) {
                nodes[x + nMapWidth * y].bVisited = false;
                nodes[x + nMapWidth * y].localDistance = std::numeric_limits<float>::infinity();
                nodes[x + nMapWidth * y].globalDistance = std::numeric_limits<float>::infinity();
                nodes[x + nMapWidth * y].parent = nullptr;
            }
        }

        std::vector<Node*> frontier;
        std::vector<Node*> A_Star;

        nodeStart->globalDistance = CalcHueristic(nodeStart, nodeEnd);
        nodeStart->localDistance = 0;

        frontier.push_back(nodeStart);

        while (!frontier.empty()) {
            Node* p = frontier.front();
            for (auto n : p->neighbors) {
                if (!p->bVisited && !p->bObstical && p->localDistance < n->localDistance) {
                    n->parent = p;
                    n->localDistance = p->localDistance + 1;
                    n->globalDistance = CalcHueristic(n, nodeEnd);
                    frontier.push_back(n);
                }
            }
            p->bVisited = true;
            frontier.erase(frontier.begin());
        }

        //Node* n = nodeEnd;
        //while (n != nodeStart) {
        //    nodeEnd->parent
        //}

    }

protected:
    virtual bool OnUserCreate() {

        nodes = new Node[nMapHeight * nMapWidth];
        for (int x = 0; x < nMapWidth; x++) {
            for (int y = 0; y < nMapHeight; y++) {
                nodes[x + nMapWidth * y] = Node{ false, false, std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), x, y, nullptr };
            }
        }
        for (int x = 0; x < nMapWidth; x++) {
            for (int y = 0; y < nMapHeight; y++) {
                if (y > 0) // NORTH
                    nodes[x + nMapWidth * y].neighbors.push_back(&nodes[x + nMapWidth * (y - 1)]);
                if (y < nMapHeight - 1) // SOUTH
                    nodes[x + nMapWidth * y].neighbors.push_back(&nodes[x + nMapWidth * (y + 1)]);
                if (x < nMapWidth - 1) // EAST
                    nodes[x + nMapWidth * y].neighbors.push_back(&nodes[(x+1) + nMapWidth * y]);
                if (x > 0) // WEST
                    nodes[x + nMapWidth * y].neighbors.push_back(&nodes[(x-1) + nMapWidth * y]);
            }
        }

        //Default Start and end
        nodeStart = &nodes[nMapHeight / 4 + nMapWidth * nMapHeight / 2 - 1];

        nodeEnd = &nodes[3*nMapHeight / 4 + nMapWidth * nMapHeight / 2];

        A_Star();

        return true;
    }

    virtual bool OnUserUpdate(float fElapsedTime) {
        int nodeSize = 9;
        int nodeBorder = 2;

        int selectedNodeX = m_mousePosX / nodeSize;
        int selectedNodeY = m_mousePosY / nodeSize;

        if (m_mouse[0].bReleased) {
            if ( (0 <= selectedNodeX < nMapWidth) && (0 <= selectedNodeY < nMapHeight)) {
                if (m_keys[VK_SHIFT].bHeld)
                    nodeStart = &nodes[selectedNodeX + nMapWidth * selectedNodeY];
                else if (m_keys[VK_CONTROL].bHeld)
                    nodeEnd = &nodes[selectedNodeX + nMapWidth * selectedNodeY];
                else
                    nodes[selectedNodeX + selectedNodeY * nMapWidth].bObstical = !nodes[selectedNodeX + selectedNodeY * nMapWidth].bObstical;
                A_Star();

            }
        }



        Fill(0, 0, ScreenWidth(), ScreenHeight(), L' ');

        for (int x = 0; x < nMapWidth; x++) {
            for (int y = 0; y < nMapHeight; y++) {
                
                for(auto n: nodes[x + y * nMapWidth].neighbors) {
                    DrawLine(x * nodeSize + nodeSize / 2, y * nodeSize + nodeSize / 2, n->x * nodeSize + nodeSize / 2, n->y * nodeSize + nodeSize / 2, PIXEL_SOLID, FG_BLUE);
                }

            }
        }
        
        for (int x = 0; x < nMapWidth; x++) {
            for (int y = 0; y < nMapHeight; y++) {
                if (nodes[x + y * nMapWidth].bObstical) {
                    Fill(x * nodeSize + nodeBorder, y * nodeSize + nodeBorder, (x + 1) * nodeSize - nodeBorder, (y + 1) * nodeSize - nodeBorder, PIXEL_SOLID, FG_DARK_GREY);
                }
                else {
                    Fill(x * nodeSize + nodeBorder, y * nodeSize + nodeBorder, (x + 1) * nodeSize - nodeBorder, (y + 1) * nodeSize - nodeBorder, PIXEL_SOLID, FG_DARK_BLUE);
                }
            }
        }

        Fill(nodeStart->x * nodeSize + nodeBorder, nodeStart->y * nodeSize + nodeBorder,
            (nodeStart->x + 1) * nodeSize - nodeBorder, (nodeStart->y + 1) * nodeSize - nodeBorder, PIXEL_SOLID, FG_RED);

        Fill(nodeEnd->x * nodeSize + nodeBorder, nodeEnd->y * nodeSize + nodeBorder,
            (nodeEnd->x + 1) * nodeSize - nodeBorder, (nodeEnd->y + 1) * nodeSize - nodeBorder, PIXEL_SOLID, FG_YELLOW);

        Node* p = nodeEnd;
        //std::cout << nodeEnd->parent->x << nodeEnd->parent->x << std::endl;
        while (p->parent != nullptr) {
            DrawLine(p->x * nodeSize + nodeSize / 2, p->y * nodeSize + nodeSize / 2, p->parent->x * nodeSize + nodeSize / 2, p->parent->y * nodeSize + nodeSize / 2, PIXEL_SOLID, FG_GREEN);
            p = p->parent;
        }

        return true;
    }
};

int main()
{
    PathFinding game;
    game.ConstructConsole(160, 160, 6, 6);
    game.Start();
    return 0;
}

