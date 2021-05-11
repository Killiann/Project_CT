#include "Track.h"
Track::Track(ResourceManager *resource, std::vector<CheckPoint> checkPoints, std::vector<std::vector<int>> grid, unsigned int trackW, unsigned int trackH) : gridVals(grid), trackGridH(trackH), trackGridW(trackW){
	
	cpManager = CheckPointManager(resource->GetCheckpointTexture(), checkPoints);

	for (unsigned int y = 0; y < trackGridH; ++y) {
		for (unsigned int x = 0; x < trackGridW; ++x) {
			if (gridVals[y][x] != -1) {
				int trackPieceID = ((gridVals[y][x] - 1) / 4);
				float rotation = ((gridVals[y][x] - 1) % 4) * 90.f;
				trackPieces.emplace_back(*resource->GetTrackTexture(), tileSize, sf::Vector2f((float)x, (float)y), (TrackType)trackPieceID, rotation);
			}
		}
	}

	//save all track shapes in one vector 
	for (auto &i : trackPieces) {
		std::vector<sf::ConvexShape> collShapes = i.GetCollisionShapes();
		trackShapes.insert(trackShapes.end(), collShapes.begin(), collShapes.end());
	}
}

void Track::Draw(sf::RenderTarget& window, bool devOverview) {
	for (auto& t : trackPieces)
		t.Draw(window);

	cpManager.Draw(window, devOverview);
}