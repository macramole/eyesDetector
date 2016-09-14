class CurrentRectangle {
	public:
		ofRectangle rect;
		const int MIN_FRAMES = 4;
		const float MIN_FRAMES_POWER = 1.4;
		const int AGE_OF_CONSENT = 6; //sera candidate hasta que cumpla AGE_OF_CONSENT
		const int MIN_INTERSECTION_AREA = 100; //mas chico hará que cuadros mas alejados sean seleccionados
		const int MIN_PROMEDIADOS_START  = 5; //luego de haber promediado por lo menos 5 cuadrados empezará a ajustarse

		const float SCALE_RECT = 1.8; //hacer mas grande la ventana para que tome mas que ojos

		int frames = 0;
		int age = 0; //sube uno cada MIN_FRAMES que pasaron
		bool hadRectanglesInside = false;
		bool isCandidate = true; //sera candidate hasta que cumpla AGE_OF_CONSENT
		int previousRectangleAge = -1; //esto me sirve para cuando grabo
		ofPoint centroPromedio;
		unsigned int cantPromediados = 0;

		const float RECTANGLE_FORCE_ASPECT_RATIO = 4.0; //4:1 aspect ratio;
		//const float RECTANGLE_FORCE_WIDTH = 440; //force to 440px

		static const int STATUS_NO_EYES = 0;
		static const int STATUS_WITH_EYES = 1;
		static const int STATUS_NEW_EYES = 2;
		static const int STATUS_AGE_UP = 3;
		static const int STATUS_AGE_OF_CONSENT_REACHED = 4;
		static const int STATUS_EYES_LOST = 5;

		bool isEmpty() {
			return ( rect.width == 0 );
		}
		/**
			Cantidad de frames del último año del rectangulo anterior
			sirve para borrar estos ultimos frames asi no me queda basurita al final
		**/
		int getPreviousRectangleLastYearNumFrames() {
			int cantFrames =
				ceil(
                    MIN_FRAMES * pow( MIN_FRAMES_POWER ,previousRectangleAge) -
					MIN_FRAMES * pow( MIN_FRAMES_POWER ,previousRectangleAge - 1)
				);

			return cantFrames;
		}

		void setRectangle(ofRectangle rectangle) {
			rect = rectangle;
		}
		void reset() {
			previousRectangleAge = age;
			rect.set(0,0,0,0);
			frames = 0;
			hadRectanglesInside = false;
			age = 1;
			isCandidate = true;
			centroPromedio.set(0,0,0);
		}
		void reset(ofRectangle newRectangle) {
			reset();
			rect = newRectangle;
			rect.setHeight( rect.getWidth() / RECTANGLE_FORCE_ASPECT_RATIO );


			rect.scaleFromCenter(SCALE_RECT);
		}
		int process( std::vector<ofxCvBlob> blobs ) {
			int returnStatus = NULL;

			//si no hay nada que agarre el primero
		    if ( isEmpty() ) {
				returnStatus = STATUS_NO_EYES;

				if ( !blobs.empty() ) {
					reset( getBiggestRectangle(blobs) );
					returnStatus = STATUS_NEW_EYES;
		        }
		    } else {
				processCentroPromedio(blobs);

				returnStatus = STATUS_WITH_EYES;

		        if ( !hadRectanglesInside ) {
		            if ( frames > MIN_FRAMES ) {
						returnStatus = STATUS_EYES_LOST;
						reset();
		            } else {
		                hadRectanglesInside = isAnyRectangleInside(blobs);
		            }
		        } else {
		            if ( frames > MIN_FRAMES * pow( MIN_FRAMES_POWER ,age) ) { //la cantidad de frames requeridos va aumentando
		                frames = -1;
		                hadRectanglesInside = false;
						age++;
						returnStatus = STATUS_AGE_UP;
						if ( age >= AGE_OF_CONSENT && isCandidate == true ) {
							returnStatus = STATUS_AGE_OF_CONSENT_REACHED;
							isCandidate = false;
						}
		            }
		        }
		        frames++;
		    }

			return returnStatus;
		}
		bool isAnyRectangleInside( std::vector<ofxCvBlob> blobs ) {
		    if ( !isEmpty() ) {
		        for(int i = 0; i < blobs.size(); i++) {
		            ofRectangle intersection = rect.getIntersection( blobs[i].boundingRect );
		            if ( intersection.getArea() > MIN_INTERSECTION_AREA ) {
		                return true;
		            }
		        }
		    }
		    return false;
		}
		ofRectangle getBiggestRectangle( std::vector<ofxCvBlob> blobs ) {
			ofRectangle * biggestRectangle = new ofRectangle();
			for(int i = 0; i < blobs.size(); i++) {
				if ( blobs[i].boundingRect.getArea() > biggestRectangle->getArea() ) {
					biggestRectangle = &blobs[i].boundingRect;
				}
			}
			return *biggestRectangle;
		}
		void processCentroPromedio( std::vector<ofxCvBlob> blobs ) {
			if ( !isEmpty() ) {
				for(int i = 0; i < blobs.size(); i++) {
					ofRectangle intersection = rect.getIntersection( blobs[i].boundingRect );
		            if ( intersection.getArea() > MIN_INTERSECTION_AREA ) {
						centroPromedio += blobs[i].boundingRect.getCenter();
						centroPromedio = centroPromedio / 2;
						cantPromediados++;
		            }
				}

				if ( cantPromediados >= MIN_PROMEDIADOS_START ) {
					ofPoint centerError = rect.getCenter().interpolate( centroPromedio, 0.1 );
					rect.setFromCenter( centerError, rect.getWidth(), rect.getHeight() );
				}
			}
		}
};
